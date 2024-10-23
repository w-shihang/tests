#!/usr/bin/python3

import csv
import fcntl
import getpass
import json
import os
import platform
import re
import subprocess
import sys
import time
import glob
import pytest
from datetime import datetime
from itertools import groupby
from socket import timeout

import pexpect
import pexpect.fdpexpect
import pexpect.spawnbase
import serial
import atexit
import threading

from utils.enum.code_enum import StatusCodeEnum

rootPath = os.path.dirname(os.path.abspath(__file__))

tmp_read_nonblocking = pexpect.spawnbase.SpawnBase.read_nonblocking

def enhanced_read_nonblocking(self, size=1, timeout=None):
    return re.sub(r'(\x9B|\x1B\[)[0-?]*[ -\/]*[@-~]', "", tmp_read_nonblocking(self, size, timeout).decode(errors="ignore")).encode()

pexpect.spawnbase.SpawnBase.read_nonblocking = enhanced_read_nonblocking


class CommandNotFoundException(Exception):
    pass


class connectNuttx(object):
    def __init__(
        self,
        board,
        path,
        dev,
        log_path,
        fs,
        ota_version,
        core,
        sudo,
        ci,
        method,
        target,
        enable_gdb
    ):
        self.board = board
        self.path = path
        self.dev = dev
        self.log_path = log_path
        self.fs = fs
        self.version = ota_version
        self.sudo = sudo
        self.ci = ci
        self.core = core
        self.method = method
        self.target = target
        self.enter = "\r"
        self.debug_flag = 0
        self.enable_gdb = enable_gdb
        self.format_str_len = 105
        self.restart_sim_count = 0
        self.lock_file_fd = None
        # get PROMPT value and rate value
        self.PROMPT = getConfigValue(
            self.path, self.board, core=self.core, flag="NSH_PROMPT_STRING"
        )
        if self.target == 'target':
            self.rate = getConfigValue(
                self.path, self.board, core='ap', flag="UART0_BAUD"
            )
            self.reboot_prompt = getConfigValue(
                self.path, self.board, core="ap", flag="NSH_PROMPT_STRING"
            )
        else:
            self.rate = getConfigValue(
                self.path, self.board, core=self.core, flag="UART0_BAUD"
            )
            self.reboot_prompt = self.PROMPT

        if not os.path.exists(self.log_path):
            os.makedirs(self.log_path)

    def setup(self):
        atexit.register(self.kill_child_process)
        if self.restart_sim_count == 0:
            self.start_time = time.strftime("%Y%m%d_%H%M%S", time.localtime())
        self.restart_sim_count += 1
        print("============ start at %s ============" % self.start_time)
        if self.target == "sim":
            # start nuttx for sim
            start.startNuttx(self, self.path, self.log_path,
                             self.board, self.sudo)
        if self.target == "qemu":
            start.startQemu(self, self.path, self.log_path,
                            self.board, self.sudo)
        if self.target == "fvp":
            start.startFvp(self, self.path, self.log_path,
                           self.board, self.sudo)
        if self.target in ["target", "module"]:
            # start minicom
            if self.method == "minicom":
                start.startMinicom(
                    self, self.dev, self.board, self.log_path, self.core, self.rate
                )
            elif self.method == "picocom":
                start.startPicocom(
                    self, self.dev, self.board, self.log_path, self.core, self.rate
                )
            else:
                start.startSerial(
                    self, self.dev, self.board, self.log_path, self.core, self.rate
                )

    def sendcontrol(self, char):
        char = char.lower()
        a = ord(char)
        if 97 <= a <= 122:
            a = a - ord("a") + 1
            byte = bytes([a])
            return byte
        d = {
            "@": 0,
            "`": 0,
            "[": 27,
            "{": 27,
            "\\": 28,
            "|": 28,
            "]": 29,
            "}": 29,
            "^": 30,
            "~": 30,
            "_": 31,
            "?": 127,
        }
        if char not in d:
            return b""
        byte = bytes(d[char])
        return byte

    def sendControlCmd(self, cmd, expect="ap>", timeout=10):
        if self.method == "minicom":
            self.process.sendcontrol(cmd)
            time.sleep(1)
            self.process.sendcontrol(cmd)
        else:
            byte = self.sendcontrol(cmd)
            time.sleep(1)
            self.process.send(byte)
            time.sleep(1)
            self.process.send(byte)

    def open_timestamp(self):
        if self.method == "minicom":
            self.process.sendcontrol("a")
            self.process.sendline("n")

    def jdq_reboot(self) -> bool:

        pattern = r"/dev/(?P<device>[a-z0-9]*[0-9])(?P<type>[a-z]*)USB"

        self.print_format_str("Try to restart device by relay...")
        self.print_format_str("Log port: " + self.dev)
        if (match := re.search(pattern, self.dev)) is not None:
            device = match.group("device")
            type = match.group("type")
            jdq = "/dev/{}_{}_jdq".format(device, type)
            self.print_format_str("Relay: " + jdq)
            if os.path.exists(jdq):
                jdqreboot = r"sudo echo -e -n '\xA0\x01\x01\xA2' > {} && sleep 0.200 ; sudo echo -e -n '\xA0\x01\x00\xA1' > {}".format(jdq, jdq)
                self.print_format_str(f"Exec: {jdqreboot}")
                os.system(jdqreboot)
                return True
        self.print_format_str("Not found relay.")
        return False

    def is_sim_exit(self):
        try:
            self.process.sendline("\n")
            ret = self.process.expect("(gdb)", timeout=1)
            if ret == 0:
                return True
        except pexpect.TIMEOUT:
            return False

    def is_crash(self):
        """
        Returns:
            False: crash not happened
            True: crash happened
        """
        self.print_format_str("Crash status checking...")
        try:
            self.process.expect_exact("Assertion", timeout=10)
        except pexpect.TIMEOUT:
            self.print_format_str("No crash occurred.")
            return False

        self.print_format_str(f"***** CRASH HAPPENED *****")
        self.debug("up_assert", 0)
        return True

    def is_busy_loop(self, retry_times=10):
        """
        Args:
            retry_times: retry times
        """
        self.print_format_str("Busy loop status checking...")
        self.print_format_str("Reading data from serial port:")
        counter = 0
        while True:
            try:
                self.process.sendline("\n")
                time.sleep(0.2)
                self.process.sendline("\n")
                time.sleep(0.2)

                output = self.process.read_nonblocking(size=16384, timeout=5).decode(errors="ignore")
                print("")
                print(output)
                print("")
                # last_line = output.split("\n")[-1]
                # self.print_format_str("Output last line: " + last_line)

                for prompt in [self.PROMPT, "ap>", "cp>", "audio>", "tee>", "mediatool>", "bttool>", "player>"]:
                    if prompt in output:
                        self.print_format_str(f"Found prompt '{prompt}', no busy loop occurred.")
                        return prompt

                counter += 1
                self.print_format_str(f"Not found any prompt after sending \"\\n\".({counter})")

            except pexpect.TIMEOUT:
                counter += 1
                self.print_format_str(f"No response from log port.({counter})")

            finally:
                # .sendline("\n") may be timeout
                if counter == retry_times:
                    self.print_format_str(f"***** BUSY LOOP HAPPENED *****")
                    return ""

    def try_to_force_panic(self) -> bool:
        '''
        Returns:
            False: force panic failed
            True: force panic successful
        '''
        self.print_format_str("Try to send a \"TTY_FORCE_PANIC_CHAR\"...")
        try:
            self.process.sendline(chr(31))
            self.process.expect_exact("Assertion", timeout=5)
            self.debug("force_panic", 0)
        except pexpect.TIMEOUT:
            self.print_format_str("Not found \"Assertion\" after sending a \"TTY_FORCE_PANIC_CHAR\".")
            return False
        self.print_format_str("Forced panic successful.")
        return True

    def print_format_str(self, string, type="text"):
        str_prefix = "+"
        str_suffix = "+"

        if type == "head":
            rest_char_len = self.format_str_len - 2 - len(string)
            half_len = int(rest_char_len / 2)
            print(str_prefix + "-" * half_len + string + "-" * (rest_char_len - half_len) + str_suffix)
        elif type == "tail":
            rest_char_len = self.format_str_len - 2
            print(str_prefix + "-" * rest_char_len + str_suffix)
        elif type == "text":
            str_prefix = "| "
            str_suffix = " |"
            rest_char_len = self.format_str_len - len(str_prefix) - len(str_suffix) - len(string)
            print(str_prefix + string + " " * (1 if rest_char_len < 1 else rest_char_len) + str_suffix)
        else:
            print(string)

    def clean_buffer(self):
        i = -1
        end_time = time.time() + 10
        while True:
            if ((self.process.before is not None and self.process.before.decode(errors="ignore").replace('\r', '').replace('\n', '') != "")
                    or (self.process.after is not None and self.process.after != pexpect.TIMEOUT and self.process.after.decode(errors="ignore").replace('\r', '').replace('\n', '') != "")
                    or i == 0):
                i = self.process.expect([re.compile(b".+"), pexpect.TIMEOUT, pexpect.EOF], timeout=0.1)
                if end_time < time.time():
                    break
            else:
                while True:
                    try:
                        self.process.read_nonblocking(size=self.process.maxread, timeout=0.1)
                        if end_time < time.time():
                            break
                    except Exception:
                        break
                break
        self.process.before = b""
        self.process.after = b""
        self.process.buffer = b""

    def sendCommandRead(self, cmd, timeout):
        time.sleep(1)
        self.clean_buffer()
        self.process.sendline(cmd)
        time.sleep(0.1)
        self.process.sendline('finish send')
        self.process.expect("finish send",timeout=timeout)
        res = self.process.before.decode()
        self.clean_buffer()
        return res

    # send command to nsh
    def sendCommand(self, cmd, *argc, **argv):
        expect = []
        original_expect = []
        timeout = 10
        flag = ""
        disable_check_hang = False
        ltp_for_sim_flag = False
        timeout_param_flag = False
        ret = StatusCodeEnum.NORMAL.value[0]
        length = len(argc)
        if length == 0:
            expect.append(self.PROMPT)
        else:
            for i in argc:
                expect.append(i)
        length = len(argv)
        if length != 0:
            for key, value in argv.items():
                if key == "timeout":
                    timeout = value
                    timeout_param_flag = True
                if key == "flag":
                    flag = value
                if key == "disable_check_hang":
                    disable_check_hang = True
                if key == "sim_ltp":
                    ltp_for_sim_flag = True

        original_expect = expect.copy()
        if ("echo $?" in cmd) and (expect[0] == "0" or expect[0] == "1"):
            if not timeout_param_flag:
                timeout = 0.5
            if self.target not in ["sim", "qemu", "fvp"]:
                expect[0] = expect[0] + "\r\n"

        try:
            if ltp_for_sim_flag:
                ret = self.dealLtpCommandForSim(cmd, expect, timeout)
            else:
                if self.method not in ["minicom", "picocom"]:
                    time.sleep(0.5)
                if self.target == "fvp":
                    self.process2.buffer = b""
                    self.process2.sendline(cmd)
                elif self.target == "qemu":
                    self.clean_buffer()
                    self.process.sendline(cmd)
                else:
                    self.clean_buffer()
                    self.process.sendline(cmd)
                    time.sleep(0.1)
                    self.process.send("\r\n\r\n")

                try:
                    ret = self.process.expect("command not found", timeout=0.1)
                    if ret == 0:
                        raise CommandNotFoundException
                except pexpect.TIMEOUT:
                    pass

                length = len(expect)
                if length == 1:
                    ret = self.process.expect(expect[0], timeout=timeout)
                else:
                    for i in range(length):
                        ret = self.process.expect(expect, timeout=timeout)
                        expect.pop(ret)

        except Exception as e:
            self.print_format_str(" Catch Exception ", type="head")

            if isinstance(e, pexpect.TIMEOUT):
                crash_flag = False
                force_panic_flag = False
                jdq_reboot_flag = False
                ret = StatusCodeEnum.TIMEOUT.value[0]

                if self.target == "sim" and self.enable_gdb and self.is_sim_exit():
                    ret = StatusCodeEnum.EOF.value[0]

                elif self.is_crash():
                    ret = StatusCodeEnum.CRASH.value[0]
                    crash_flag = True

                elif not disable_check_hang:
                    prompt = self.is_busy_loop(retry_times=10)
                    if prompt == "":
                        ret = StatusCodeEnum.BUSYLOOP.value[0]
                        if self.target not in ["sim", "qemu"]:
                            force_panic_flag = self.try_to_force_panic()
                            if not force_panic_flag:
                                jdq_reboot_flag = self.jdq_reboot()
                    elif prompt == self.PROMPT or flag in prompt:
                        ret = StatusCodeEnum.TIMEOUT.value[0]
                    else:
                        self.switch_to_original_core(flag)
                        ret = StatusCodeEnum.TIMEOUT.value[0]

                if crash_flag or force_panic_flag or jdq_reboot_flag:
                    try:
                        self.process.expect_exact("NSH", timeout=20)
                        self.reboot_check(self.reboot_prompt)
                    except Exception:
                        self.print_format_str(f"Restart {self.board}, but not found \"NSH\" or reboot check fail.")

            elif isinstance(e, CommandNotFoundException):
                ret = StatusCodeEnum.COMMAND_NOT_FOUND.value[0]
                self.print_format_str(f"{cmd.split()[0]}: command not found")

            elif isinstance(e, pexpect.EOF):
                ret = StatusCodeEnum.EOF.value[0]
                self.print_format_str(f"pexpect.EOF error occurred: {str(e)}")

            elif isinstance(e, AttributeError):
                if "EOF" in str(e):
                    ret = StatusCodeEnum.EOF.value[0]
                else:
                    ret = StatusCodeEnum.UNKNOWN.value[0]
                self.print_format_str(f"AttributeError: {str(e)}")

            else:
                ret = StatusCodeEnum.UNKNOWN.value[0]
                self.print_format_str(f"unexpected error occurred: {str(e)}")

            self.print_format_str(" Result ", type="head")
            self.print_format_str(f"Command     : '{cmd}'")
            self.print_format_str(f"Expect value: {str(original_expect)}")
            self.print_format_str(f"Timeout     : {timeout}s")
            self.print_format_str(f"Test result : {StatusCodeEnum.get_enum_msg_by_code(ret)}")
            self.print_format_str(f"Status code : {ret}")
            self.print_format_str("", type="tail")

        finally:
            self.debug(cmd, ret)
            if self.method not in ["minicom", "picocom"]:
                time.sleep(0.5)

            if self.target == "sim":
                if ret == StatusCodeEnum.EOF.value[0]:
                    if self.enable_gdb:
                        self.process.sendline("bt")
                        time.sleep(1)
                        self.process.sendline("q")
                        time.sleep(1)
                        self.process.sendline("y")
                    self.setup()
                elif ret == StatusCodeEnum.BUSYLOOP.value[0]:
                    os.system(f"sudo kill -9 {self.process.pid}")
                    time.sleep(1)
                    if self.enable_gdb:
                        self.process.sendline("q")
                        time.sleep(1)
                        self.process.sendline("y")
                    self.setup()

            return ret

    def dealLtpCommandForSim(self, cmd, expect_value, timeout):
        output: str = ""
        try:
            start_time = time.time()
            output = ""
            for i in range(2):
                while True:
                    self.process.sendline("\n")
                    time.sleep(0.01)
                    output_tmp = self.process.read_nonblocking(size=128, timeout=5).decode(errors="ignore").replace("[K", "")
                    output = output + output_tmp
                    if "(gdb)" in output:
                        raise pexpect.EOF("EOF")
                    output_list = output.split("\n")
                    if self.PROMPT in output_list[-1]:
                        break
                    if time.time() - start_time >= 2:
                        raise pexpect.TIMEOUT("Timeout")

            end_time = 0
            output = ""
            if timeout is not None:
                end_time = time.time() + timeout

            self.process.sendline(cmd)
            while True:
                if (timeout is not None) and (timeout < 0):
                    raise pexpect.TIMEOUT("Timeout")

                output_tmp = self.process.read_nonblocking(size=4096, timeout=timeout).decode(errors="ignore").replace("[K", "")
                output = output + output_tmp

                if "(gdb)" in output:
                    raise pexpect.EOF("EOF")

                if "command not found" in output:
                    raise CommandNotFoundException

                if isinstance(expect_value[0], list):
                    expect_list = expect_value[0]
                else:
                    expect_list = expect_value
                for index, item in enumerate(expect_list):
                    if item in output:
                        return index

                if timeout is not None:
                    timeout = end_time - time.time()
        except KeyboardInterrupt:
            pytest.exit("KeyboardInterrupt")

    def switch_to_original_core(self, core=""):
        if self.target == "target":
            if core == ">" or "mediatool" in core or "bttool" in core:
                return
            elif core == "":
                target_core = self.core
            elif core.endswith(">"):
                target_core = core[:-1]
            else:
                target_core = core

            self.print_format_str(f"Switch to target core:{target_core}")
            for i in range(3):
                self.sendControlCmd("c")
                if target_core != "ap":
                    self.process.sendline(f"cu -l /dev/tty{target_core.upper()}\n\n")

                try:
                    self.process.sendline("\n\n")
                    self.process.expect(f"{target_core}>", timeout=5)
                except pexpect.TIMEOUT:
                    self.debug(f"switch {target_core}", -1)
                    time.sleep(1)
                    continue
                self.print_format_str(f"Switch to {target_core} successful.")
                return
            self.print_format_str(f"Switch to {target_core} failed.")

    def reboot_check(self, prompt):
        """
        Check if the reboot success
        """
        # flush more reboot log in buffer
        if self.board in pair:
            try:
                ret = self.process.expect(pair[self.board], timeout=50)
                self.debug("reboot2", ret)
            except:
                print("no more reboot log")
        # makesure the reboot is successful
        self.process.buffer = b""
        self.process.sendline("\n")
        is_reboot = self.process.expect_exact(prompt)
        self.debug("reboot3", is_reboot)
        self.switch_to_original_core()

    def reboot(self, *args, **kwargs):
        if self.board in ["sim"]:
            return
        if self.board == "86v1":
            expect = "run rcS.last"
        else:
            expect = "NSH"
        cmd = "reboot"
        if kwargs.get('crash', False):
            cmd = "mw -1"
        # execute cmd
        self.sendControlCmd("c")
        ret = self.sendCommand(cmd, expect, timeout=50, flag=self.reboot_prompt, disable_check_hang=True)
        self.reboot_check(self.reboot_prompt)
        assert ret == 0

    def debug(self, cmd, ret):
        if self.debug_flag:
            print("********************* DEBUG START ********************")
            if cmd == "\n":
                cmd = r"\n"
            print("cmd: {}".format(cmd))
            print("ret: {}".format(ret))
            print("before: {}".format(self.process.before.decode(errors="ignore")))
            print("after: {}".format(self.process.after.decode(errors="ignore")))
            print("buffer: {}".format(self.process.buffer.decode(errors="ignore")))
            print("********************** DEBUG END **********************")

    def kill_child_process(self):
        if self.process is not None and self.process.isalive():
            atexit.unregister(self.kill_child_process)
            print(f"sudo kill -9 {self.process.pid}.")
            os.system(f"sudo kill -9 {self.process.pid}")

    def cleanup(self):
        if self.target == "sim":
            self.process.sendline("poweroff")
            if self.enable_gdb:
                time.sleep(1)
                self.process.sendline("q")
                time.sleep(1)
                self.process.sendline("y")
        elif self.target == "qemu":
            if self.board == "goldfish-armeabi-v7a-ap":
                self.process.sendline("poweroff")
            else:
                self.sendControlCmd("a", self.PROMPT)
                self.process.sendline("x")
        elif self.target == "fvp":
            self.process.sendcontrol(']')
            self.process.sendline("q")
            self.process2.sendline("c")
        elif self.method == "picocom":
            self.process.sendcontrol('c')
        else:
            killPid("minicom", device=self.dev.split("/")[-1])

        self.kill_child_process()

    def sendCommand2(self, cmd, type):
        core = self.core + "_performance_test"
        l1 = findFile(self.path, "elf.log", self.board, core=core)
        l2 = findFile(self.path, "arm-none-eabi-readelf.log",
                      self.board, core=core)
        if l1 == [] or l2 == []:
            start.startOtherBash(
                self, self.path, self.log_path, self.board, cmd)
            l1 = findFile(self.path, "elf.log", self.board, core=core)
            l2 = findFile(self.path, "arm-none-eabi-readelf.log",
                          self.board, core=core)
        with open(l1[0], "r+") as f:
            lines = f.readlines()
        f.close()
        size = None
        addr = None
        if type == "sram":
            for line in lines:
                if cmd in line:
                    values = re.split(r'\s+|\t', line.strip())
                    x = []
                    for value in values:
                        if len(value) == 8:
                            x.append(value)
                    return (x[0], x[1])
        elif type == "nor_flash":
            with open(l2[0], "r+") as f:
                lines = f.readlines()
            f.close()
            for line in lines:
                values = re.split(r'\s+|\t', line.strip())
                if len(values) <= 10:
                    continue
                i = 0
                while i < len(values):
                    if ".text" in values[i]:
                        return (values[i + 2], values[i + 4])
                    i += 1
        return (addr, size)

    def acquire_lock(self, lock_file) -> bool:
        fd = os.open(lock_file, os.O_RDWR | os.O_TRUNC | os.O_CREAT)
        try:
            fcntl.flock(fd, fcntl.LOCK_EX | fcntl.LOCK_NB)
            atexit.register(self.release_lock)
            print(f"Process {os.getpid()} acquired lock successfully.")
        except IOError as e:
            os.close(fd)
            print(e)
            print(f"Process {os.getpid()} failed to acquire lock.")
            return False
        else:
            self.lock_file_fd = fd
            return True

    def release_lock(self) -> bool:
        try:
            fcntl.flock(self.lock_file_fd, fcntl.LOCK_UN)
            os.close(self.lock_file_fd)
            print(f"Process {os.getpid()} released lock.")
            atexit.unregister(self.release_lock)
            return True
        except IOError as e:
            print(e)
            print(f"Process {os.getpid()} releasing lock error.")
            return False

    def release_lock_after_delay(self, delay) -> bool:
        print(f"Lock will be released in {delay}s.")
        time.sleep(delay)
        return self.release_lock()

    def run_after_get_lock(self):
        wait_time_for_retry = 10
        while True:
            if self.acquire_lock("/tmp/qemu.lock"):
                break
            else:
                print(f"Try to acquire lock in {wait_time_for_retry}s.")
                time.sleep(wait_time_for_retry)

        thread = threading.Thread(target=self.release_lock_after_delay, args=(60,))
        thread.daemon = True
        thread.start()

        self.process = pexpect.spawn(
            "bash",
            [
                "-c",
                "./emulator.sh  -no-window | tee %s" % self.log,
            ],
        )


class start:
    def startMinicom(self, dev, board, log_path, core, rate):
        self.log = "{}/{}_{}.cap".format(log_path, dev[-4:], self.start_time)
        self.process = pexpect.spawn(
            r"sudo minicom -D {} -b {} -o -C {}".format(dev, rate, self.log),
            maxread=200000,
        )
        self.process.expect("minicom")
        self.switch_to_original_core()

    def startPicocom(self, dev, board, log_path, core, rate):
        self.log = "{}/{}_{}.log".format(log_path, dev[-4:], self.start_time)
        self.process = pexpect.spawn("sudo picocom {} -b {} --logfile {}".format(dev, rate, self.log), maxread=200000)
        self.process.expect("Terminal ready")
        self.switch_to_original_core()

    def startSerial(self, dev, board, log_path, core, rate):
        self.log = "{}/{}_{}.cap".format(log_path, dev[-4:], self.start_time)
        self.logFile = open(self.log, "ab+")
        self.ser = serial.Serial(port=dev, baudrate=int(rate))
        self.process = pexpect.fdpexpect.fdspawn(
            self.ser, "wb", maxread=20000, logfile=self.logFile
        )
        self.switch_to_original_core()

    def startNuttx(self, path, log_path, board="sim", sudo=False):
        os.chdir(path)
        self.log = "{}/{}_{}.log".format(log_path, board, self.start_time)
        with open(self.log, "ab+") as f:
            log_head = "\n\n========== " + f"Start Sim {self.restart_sim_count}" + " ==========\n\n"
            f.write(log_head.encode())

        os.environ["DISPLAY"] = ":0.0"
        if self.enable_gdb:
            print("start sim by gdb")
            self.process = pexpect.spawn("bash", ["-c", f"gdb ./nuttx | tee -a {self.log}"])
            self.process.expect("(gdb)", timeout=5)
            self.process.sendline("run")
            time.sleep(2)
            self.process.sendline("\n")
        elif not sudo:
            if board in ["sim_rpserver", "sim_rpproxy"]:
                os.chdir(path)
                self.process = pexpect.spawn(
                    "bash", [
                        "-c", "sudo ./rpserver/nuttx/nuttx | tee -a %s" % self.log]
                )
            else:
                self.process = pexpect.spawn(
                    "bash", ["-c", "sudo ./nuttx | tee -a %s" % self.log]
                )
        else:
            self.process = pexpect.spawn(
                "bash", ["-c", "./nuttx | tee -a %s" % self.log])
        self.process.delayafterread = None
        self.process.expect(self.PROMPT)
        time.sleep(2)
        self.clean_buffer()
        self.process.sendline("\n")
        self.process.expect(self.PROMPT)
        self.clean_buffer()

    def startQemu(self, path, log_path, board="qemu", sudo=False):
        os.chdir(path)
        self.log = "{}/{}_{}.log".format(log_path, board, self.start_time)
        arch_chip = getConfigValue(path, board, core=None, flag="ARCH_CHIP")
        if arch_chip == "imx6":
            self.process = pexpect.spawn(
                "bash",
                [
                    "-c",
                    "qemu-system-arm -M sabrelite -smp 1 -bios none -kernel ./nuttx -nographic | tee %s"
                    % self.log,
                ],
            )
        elif arch_chip == "qemu-rv":
            flag2 = getConfigValue(path, board, core=None, flag="ARCH_RV64")
            if flag2:
                riscv = "qemu-system-riscv64"
            else:
                riscv = "qemu-system-riscv32"
            self.process = pexpect.spawn(
                "bash",
                [
                    "-c",
                    "%s -M virt -bios ./nuttx -nographic | tee %s" % (
                        riscv, self.log),
                ],
            )
        elif arch_chip == "qemu":
            flag2 = getConfigValue(path, board, core=None, flag="SMP")
            smp = ""
            if flag2:
                smp = "-smp 4"
            self.process = pexpect.spawn(
                "bash",
                [
                    "-c",
                    "qemu-system-aarch64 -cpu cortex-a53 %s \
                    -nographic \
                    -machine virt,virtualization=on,gic-version=3 \
                    -net none -kernel ./nuttx | tee %s"
                    % (smp, self.log),
                ],
            )
        elif arch_chip == "goldfish":
            if board == "qemu":
                self.run_after_get_lock()
            else:
                self.process = pexpect.spawn(
                    "bash",
                    [
                        "-c",
                        "./emulator.sh -no-window | tee %s" % self.log,
                    ],
                )
        self.process.delayafterread = None
        self.process.expect(self.PROMPT, timeout=180)

    def getPortid(content):
        start = content.find("terminal_1")
        if start != -1:
            start += len("terminal_1")
        else:
            return None
        end = content.find("terminal_2:", start)
        if end == -1:
            return None

        result = content[start:end].strip()
        # result = Listening for serial connection on port 5001
        pattern = r"port (\d+)"
        result = re.search(pattern, result)
        if result:
            return result.group(1)
        else:
            return None

    def startFvp(self, path, log_path, board="fvp", sudo=False):
        """
        child : the first bash to get port_id
        nuttx_path : where is nuttx
        fvp_path : where is AEMv8R_FVP/AEMv8R_base_pkg/models/Linux64_GCC-9.3/FVP_BaseR_AEMv8R
        """
        os.chdir(path)
        flag2 = getConfigValue(path, board, core=None, flag="SMP")
        txt_name = "fvp_cfg.txt"
        if flag2:
            txt_name = "fvp_cfg_smp.txt"
        boards_txt = "./boards/arm64/fvp-v8r/fvp-armv8r/scripts/" + txt_name
        self.log = "{}/{}_{}_1.log".format(log_path, board, self.start_time)
        nuttx_bin = "./nuttx"
        fvp_path = "FVP_BaseR_AEMv8R"
        command = fvp_path + ' -f ' + boards_txt + \
            ' -a ' + nuttx_bin + " | tee " + self.log
        self.process = pexpect.spawn(
            "bash",
            [
                "-c",
                command,
            ],
        )
        flag = 0
        file_data = ""

        while True:
            try:
                self.process.expect('\n', timeout=1)
                file_data += str(self.process.before)
            except pexpect.TIMEOUT:
                if flag == 0:
                    port_id = start.getPortid(file_data)
                    if port_id:
                        flag = 1
                        break
                else:
                    break
            except pexpect.EOF:
                print('=============pexpect.EOF=================')

        command = "telnet localhost " + port_id
        self.log2 = "{}/{}_{}_2.log".format(log_path, board, self.start_time)
        command = command + " | tee " + self.log2
        self.process2 = pexpect.spawn(
            "bash",
            [
                "-c",
                command,
            ],
        )
        self.process2.expect(self.PROMPT)

    def startOtherBash(self, path, log_path, board, cmd):
        path = path + '/' + board + '/' + self.core + "_performance_test"
        os.chdir(path)
        log_name = [
            "{}/elf.log".format(path), "{}/arm-none-eabi-readelf.log".format(path)]
        command_name = ["objdump -t nuttx | tee ",
                        "arm-none-eabi-readelf -S nuttx | grep text | tee "]

        i = 0
        while i < len(log_name):
            command = command_name[i] + log_name[i]
            self.process2 = pexpect.spawn(
                "bash",
                [
                    "-c",
                    command,
                ],
            )

            while True:
                try:
                    self.process2.expect('\n', timeout=1)
                except pexpect.TIMEOUT:
                    break
                except pexpect.EOF:
                    self.process2.close()
                    break
            i += 1


def runCmd(cmd):
    p = subprocess.Popen(
        cmd,
        shell=True,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )

    stdout, stderr = p.communicate()
    recode = p.returncode

    if recode != 0:
        print("Debug: run command '%s' failed" % cmd)

    return stdout


def getResult(case, ret, dic):
    if ret == 0:
        dic[case] = "PASS"
    else:
        dic[case] = "FAIL"

    return dic


# read case from builtin_list.h file
def getCaseList(buildinfile, board, flag):
    f = open(buildinfile, "r")
    lines = f.readlines()
    f.close()
    clist = []
    for line in lines:
        if flag in line:
            # line: { "ltp_interfaces_mq_setattr_1_1", SCHED_PRIORITY_DEFAULT, 8192, ltp_test_interfaces_mq_setattr_1_1_main },
            # get ltp_interfaces_mq_setattr_1_1
            obj = re.search(r'.*("%s.*")(.*)' % flag, line, re.M | re.I)
            clist.append(obj.group(1))
    return clist


# find crash string on filename
def findString(string, filename):
    with open(filename, "rb") as f:
        lines = f.readlines()
        f.close()

        for line in lines:
            if bool(re.search(string, str(line), re.IGNORECASE)):
                return True
            else:
                continue
        return False


# find file
def findFile(path, flag, board, core=None, match=True):
    fList = []
    for root, dirs, files in os.walk(path):
        for name in files:
            if not match and flag in name:
                fList.append(os.path.join(root, name))
            if match and name == flag and not ".o" in name:
                fList.append(os.path.join(root, name))

        if len(fList) != 1:
            fList = [x for x in fList if board in x]
    # print(fList)
    return fList


# get CONFIG_NSH_PROMPT_STRING value
def getConfigValue(path, board, core, flag):
    value = ""
    if '/out' not in path and '/images' not in path:
        core = None
    l1 = findFile(path, ".config", board, core=core)
    if not l1:
        l1 = findFile(path, ".config", board, core='%s_test' % core)
    with open(l1[0], "r+") as f:
        lines = f.readlines()
    f.close()
    for line in lines:
        if flag + "=" in line:
            value = line.split("=")[1]
        if '"' in value:
            value = value.strip('"').strip()
    print(value)
    return value


# get test suite name
def getTsName(tsFile):
    # get test suite name from tsfile
    # nsh_test_result_20201027.csv
    # ts name: nsh_test
    ts = "_".join(os.path.basename(tsFile).split("_")[:-2])
    return ts


# get case list from buildin_list.h and write to testsuite_list.txt
def readBuildInList(path, log, board, core, flag=None):
    flist = findFile(path, "builtin_list.h", board, core)
    today = time.strftime("%Y%m%d", time.localtime())
    for bfile in flist:
        if flag:
            l1 = getCaseList(bfile, board, flag)

            # get test suite list
            tsList = [list(g) for k, g in groupby(
                l1, lambda x: "%s" % flag in x) if k]
            tsFile = "%s_list_%s.txt" % (flag, today)
            if tsList:
                writeListToFile(os.path.join(log, tsFile), tsList)

        else:
            # get ltp test case list
            l1 = getCaseList(bfile, board, "ltp")
            # get open_posix list
            opList = [
                list(g)
                for k, g in groupby(l1, lambda x: "%s" % "ltp_syscalls" in x)
                if not k
            ]
            opFile = "%s_list_%s.txt" % ("open_posix", today)
            if opList:
                writeListToFile(os.path.join(log, opFile), opList)

            # get syscalls list
            sysList = [
                list(g)
                for k, g in groupby(l1, lambda x: "%s" % "ltp_syscalls" in x)
                if k
            ]
            sysFile = "%s_list_%s.txt" % ("ltp_syscalls", today)
            if sysList:
                writeListToFile(os.path.join(log, sysFile), sysList)

    return


# write runlist to file
def writeListToFile(filename, newlist):
    if newlist:
        f = open(filename, "w")
        tmplist = set([i for j in newlist for i in j])
        for ts in tmplist:
            f.write(ts.strip('"'))
            f.write("\n")
        f.close()
        return f


# read test case status and generate summary
def genSummary(dic):
    passNum = 0
    failNum = 0

    for key in dic.keys():
        if dic[key] == "PASS":
            passNum = passNum + 1
        else:
            failNum = failNum + 1
    passRate = round(passNum / (passNum + failNum) * 100, 2)
    return (passNum, failNum, passRate)


def killPid(process, device=False, sudo=False):
    if device:
        cmd = (
            "ps -aux |grep -i %s |grep %s |grep %s|grep -v grep|awk -F ' ' '{print $2}'"
            % (process, "sudo", device.split("/")[-1])
        )
    if sudo:
        cmd = "ps -aux |grep -i %s |grep %s|grep -v grep|awk -F ' ' '{print $2}'" % (
            process,
            "sudo",
        )
    if not sudo and not device:
        cmd = """ ps -aux |grep -i "%s"|grep -v grep|awk -F ' ' '{print $2}' """ % (
            process
        )
    stdout = runCmd(cmd)
    pidList = str(stdout, encoding="utf-8")
    for pid in pidList.split("\n"):
        user = getpass.getuser()
        ret = runCmd("groups %s|grep sudo; echo $?" % user).decode("utf-8")
        if pid and "0" not in ret:
            print("kill %s, normal user %s" % (pid, user))
            os.system("kill -9 %s" % pid)
        if pid and "0" in ret:
            print("kill %s, sudo user %s" % (pid, user))
            os.system("sudo kill -9 %s" % pid)


# read log and extract timestamp
def getTimestamp(filename, str1, str2, pattern):
    with open(filename, "r") as f:
        buff = f.read()
        pat = re.compile(str1 + "(.*?)" + str2, re.S)
        result = pat.findall(buff)
        timestamp = re.findall(pattern, str(result))  # extract timestamp
        return timestamp


# calculate the time difference for each time
def getDif(timestamp, rateANDinterance):
    lst = []
    for i in range(1, len(timestamp)):
        difts = int(timestamp[i]) - int(timestamp[i - 1])
        float_num = (difts / 1000000) - rateANDinterance
        lst.append(float_num)
    return lst


# ota and ot time statistics
def getTime(logfile):
    with open(logfile, "rb") as f:
        lines = [l.decode("utf8", "ignore") for l in f.readlines()]
        f.close()
    systemList = [
        "Verify image hash successfully",
        "Verify image signature successfully",
        "Recovery succeeded",
    ]
    timestamp_done = []
    sys_recovery = 0
    sys_signature = 0
    sys_hash = 0
    sys_ot = 0
    for line in lines:
        if "Recovery Service start!" in line and "ota>" in line:
            obj = re.search(r".*(\[.*\])\s*(\[.*\])\s*(\[.*\])", line)
            recovery_start = obj.group(1).strip("[").strip("]").strip()
        if "miio_client_func: OT agent listen fd" in line:
            obj = re.search(r".*(\[.*\])\s*(\[.*\])\s*(\[.*\])", line)
            ot_startTime = obj.group(1).split()[1].strip(
                "[").strip("]").strip()
        if "miio_instance: miio_online_hook_" in line:
            obj = re.search(r".*(\[.*\])\s*(\[.*\])\s*(\[.*\])", line)
            ot_endTime = obj.group(1).split()[1].strip("[").strip("]").strip()
        for systemLine in systemList:
            if systemLine in line:
                recovery_done = (
                    r"\[\s*(\d+\.?\d*)\]\s*\[\s*\d+?\]\s*\[\s*\w+?\]\s*%s" % systemLine
                )
                ll = re.findall(recovery_done, line)
                if len(ll) != 0:
                    timestamp_done.append(ll[0])
                    continue
    if len(timestamp_done) == 3:
        sys_recovery = round(
            (float(timestamp_done[2]) - float(recovery_start)), 3)
        sys_signature = round(
            (float(timestamp_done[1]) - float(recovery_start)), 3)
        sys_hash = round((float(timestamp_done[0]) - float(recovery_start)), 3)
        sys_ot = round((float(ot_endTime) - float(ot_startTime)), 3)
        # print('recovery_time = %s seconds' % sys_recovery)
        # print('hash_time = %s seconds' % sys_hash)
        # print('signature_time = %s seconds' % sys_signature)
        # print('ot_reconnect_time = %s seconds' % sys_ot)
        a = int(sys_ot)
        b = int(sys_recovery)
        if a <= 30 and b <= 600:
            print(0)
            return 0
        else:
            print(a, b)
            return 1


def connect(connect_log):
    with open(connect_log, "rb") as f:
        lines = [l1.decode("utf8", "ignore") for l1 in f.readlines()]
        f.close()
    for i in range(len(lines)):
        if (
            "packets transmitted" in lines[i].strip()
            and "packet loss" in lines[i].strip()
        ):
            # match loss packet rate
            lossRate = lines[i].split(",")[2].split()[0].strip("%")
            a = int(lossRate)
            if a != 0:
                return 0
            else:
                return a


def ip_addr(ip_addr_log):
    with open(ip_addr_log, "rb") as f:
        lines = [l1.decode("utf8", "ignore") for l1 in f.readlines()]
        f.close()
    for i in range(len(lines)):
        if "wlan0" in lines[i].strip() and "Ethernet" in lines[i].strip():
            # match wlan0 ipaddr
            ipAddr = lines[i + 1].split()[1].split(":")[1]
    return ipAddr


def read_properties(settings):
    with open(
        os.path.dirname(os.path.abspath(__file__)) + "/properties.json",
        "r",
        encoding="utf8",
    ) as fp:
        json_data = json.load(fp)
        proper = json_data[settings]
    return proper


class PullFile:
    def networking(self, p, net="2.4G"):
        # determine if it is connected to the Internet
        wifi = read_properties("wifi")
        wifi = wifi[net]
        result = 0
        expected = "nsh>"
        linkup = "nsh>"
        if p.target == "target":
            if p.core not in ["ap", "cp"]:
                p.sendControlCmd("c")
                expected = "ap>"
            else:
                expected = "%s>" % p.core
        start_time = time.time()
        ret = p.sendCommand("ifup wlan0", "ifup wlan0...OK",
                            timeout=30, flag=expected)
        result += ret
        #assert ret == 0
        ret = p.sendCommand("wapi mode wlan0 2", expected, flag=expected)
        result += ret
        #assert ret == 0
        p.sendCommand("wapi psk wlan0 " +
                      wifi["pwd"] + " 3", expected, flag=expected)
        ret = p.sendCommand(
            "wapi essid wlan0 " + wifi["essid"] + " 1", expected, timeout=30, flag=expected
        )
        result += ret
        #assert ret == 0
        p.sendCommand("renew wlan0", expected, flag=expected)
        end_time = time.time()
        calTime = round(end_time - start_time, 2)
        print("Debug: configure network time = %s seconds" % calTime)

        ret = p.sendCommand("ifconfig", wifi["epected_ip"], flag=expected)
        result += ret
        #assert ret == 0
        ret = p.sendCommand(
            "ping www.baidu.com", "bytes from", flag=expected, timeout=30
        )
        p.sendCommand(
            "ping " + wifi["pc_ip"], "bytes from", timeout=60, flag=expected
        )
        if p.target == "target":
            if p.core not in ["ap", "cp"]:
                p.switch_to_original_core()
        assert result == 0

    def connect_scp(self, p, src, dest, dest_psk):
        p.sendControlCmd("c")
        p.process.sendline("scp {} {}".format(src, dest))
        p.process.expect_exact("SHA256")
        p.process.sendline("yes")
        p.process.expect_exact("do you agree ?")
        p.process.sendline("\n")
        p.process.expect_exact("try a specific key?")
        p.process.sendline("\n")
        p.process.expect_exact("Password")
        p.process.sendline(dest_psk)
        p.process.expect_exact("wrote", timeout=10)
        p.sendCommand("\n")


# read log
def getLog(filename, str1, str2):
    l1 = []
    time.sleep(30)
    with open(filename, "r", encoding="utf-8", errors="ignore") as f:
        buff = f.read()
        f.close()
    pat = re.compile(str1 + "(.*?)" + str2, re.S)
    result = pat.findall(buff)

    if "gtest" in str1:
        return result[0].strip("\n")
    else:
        # add line to l1
        for i in result[0].strip("\n").split("\n"):
            l1.append(i)
        return l1


def genCunitJson(p):
    cunit_dic = {}
    cunit_dic["report"] = {}
    cunit_dic["report"]["summary"] = {}
    cunit_dic["report"]["tests"] = []
    cunit_dic["report"]["created_at"] = 0
    cunit_dic["report"]["summary"]["num_tests"] = 0
    cunit_dic["report"]["summary"]["passed"] = 0
    cunit_dic["report"]["summary"]["failed"] = 0
    cunit_dic["report"]["summary"]["skipped"] = 0
    cunit_dic["report"]["summary"]["duration"] = 0

    if p.core == "tee":
        with open(p.log, "r") as f:
            lines = f.readlines()
            f.close()
    else:
        lines = getLog(p.log, "cunittest -", "Elapsed Time:")
    dt = datetime.now()
    cunit_dic["report"]["created_at"] = dt.strftime("%Y-%m-%d %H:%M:%S.%f")

    for line in lines:
        dic = {}
        if "Elapsed Time" in line:
            duration = line.split(":")[1].split("(")[0].strip(" ")
        if "Tests" in line and ":" in line:
            cunit_dic["report"]["summary"]["num_tests"] = int(
                line.split(":")[1].split()[0]
            )
            cunit_dic["report"]["summary"]["failed"] = int(
                line.split(":")[1].split()[1]
            )
            Inactive = line.split(":")[1].split()[2]
            cunit_dic["report"]["summary"]["skipped"] = int(
                line.split(":")[1].split()[3]
            )
            cunit_dic["report"]["summary"]["passed"] = (
                cunit_dic["report"]["summary"]["num_tests"]
                - cunit_dic["report"]["summary"]["failed"]
                - cunit_dic["report"]["summary"]["skipped"]
            )
        if not " => " in line:
            continue
        # [  368.480000] [  INFO] [Run   Done]: syscallCloseTest01 => PASS
        case = line.split(": ")[1].split("=>")[0].strip(" ")
        className = ""
        if "syscall" in case:
            className = "SyscallTest"
        if "fileSystem" in case:
            className = "FilesystemTest"
        if "sched" in case:
            className = "ScheduleTest"
        if "Mm" in case:
            className = "MemTest"
        if "kvdb" in case:
            className = "KvdbTest"

        dic["className"] = className
        dic["methodName"] = case
        dic["spendTime"] = "None"
        dic["flag"] = p.log.split("/")[-4]
        dic["board"] = p.board
        dic["core"] = p.log.split("/")[-2]
        if "PASS" in line.split("=>")[1].strip("\n").strip(" "):
            dic["status"] = "Passed"
        if "FAIL" in line.split("=>")[1].strip("\n").strip(" "):
            dic["status"] = "Failed"
        dic["log"] = p.log

        cunit_dic["report"]["tests"].append(dic)

    with open(os.path.join(os.path.dirname(p.log), "cunit.json"), "w+") as f:
        json.dump(cunit_dic, f)
    f.close()


def rmfile(p, core, file):
    if p.core == core:
        p.sendCommand("rm -r" + file)
    else:
        p.process.sendline("cu -l /dev/tty" + core.upper())
        p.sendCommand("\n", core, flag=core + ">")
        p.sendCommand("rm -r " + file, core, flag=core + ">")
        if p.core == "ap":
            p.sendControlCmd("c")
        else:
            p.switch_to_original_core(p)


def gen_csv(p, head, data, board, core, kind):
    path_parts = p.log_path.split(os.sep)
    for i in range(len(path_parts)):
        folder_name = path_parts[i]
        # find 20230616xxxx
        if len(folder_name) == 12 and folder_name.isdigit():
            # where is performance
            per_folder_path = os.path.join('/', *path_parts[:i+1], 'performance')
            if not os.path.exists(per_folder_path):
                os.makedirs(per_folder_path)
    csv_file = os.path.join(per_folder_path,board+"_"+core+"_"+"performance_"+kind+"_data.csv")
    with open(csv_file, 'w', encoding='UTF8', newline='') as f:
        writer = csv.writer(f)
        # write the header
        writer.writerow(head)
        # write the data
        writer.writerows(data)
