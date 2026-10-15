import os
import shutil
import subprocess
import time
from pathlib import Path

Import("env")  # noqa: F821

proj = Path(env["PROJECT_DIR"])  # noqa: F821
app_dir = proj / env.GetProjectOption("custom_app_dir")  # noqa: F821
target = env.GetProjectOption("custom_app_target")  # noqa: F821

HOME = Path.home()
SDK = os.environ.get("ANDROID_SDK_ROOT", str(HOME / "Android/Sdk"))
NDK = os.environ.get("ANDROID_NDK_ROOT", f"{SDK}/ndk/27.2.12479018")
QT_ANDROID = os.environ.get("QT_ANDROID", str(HOME / "Qt/6.12.0/android_arm64_v8a"))
QT_HOST = os.environ.get("QT_HOST", str(HOME / "Qt/6.12.0/gcc_64"))
JAVA_HOME = os.environ.get("JAVA_HOME", "/usr/lib/jvm/java-17-openjdk")
ADB = f"{SDK}/platform-tools/adb"

PKG = "org.qtproject.example.wave52_client"
ACTIVITY = f"{PKG}/org.qtproject.qt.android.bindings.QtActivity"

LINUX_BUILD = app_dir / "build-linux"
ANDROID_BUILD = app_dir / "build-android"
JENV = {"JAVA_HOME": JAVA_HOME}


def run(cmd, cwd=app_dir, extra_env=None, check=True):
    e = dict(os.environ)
    if extra_env:
        e.update(extra_env)
    print(">>", " ".join(map(str, cmd)))
    return subprocess.run(cmd, cwd=str(cwd), env=e, check=check)


# linux
def build_linux(*_a, **_k):
    LINUX_BUILD.mkdir(exist_ok=True)
    run(["cmake", "-S", ".", "-B", "build-linux"])
    run(["cmake", "--build", "build-linux", "-j", str(os.cpu_count())])


def run_linux(*_a, **_k):
    build_linux()
    run(["./wave52_client"], cwd=LINUX_BUILD)


# android
def build_android(*_a, **_k):
    if not (ANDROID_BUILD / "build.ninja").exists():
        run([f"{QT_ANDROID}/bin/qt-cmake", "-S", ".", "-B", "build-android", "-G", "Ninja",
             f"-DQT_HOST_PATH={QT_HOST}",
             f"-DANDROID_SDK_ROOT={SDK}",
             f"-DANDROID_NDK_ROOT={NDK}",
             "-DANDROID_ABI=arm64-v8a",
             "-DANDROID_PLATFORM=android-31",
             "-DCMAKE_BUILD_TYPE=Debug"], extra_env=JENV)
    run(["cmake", "--build", "build-android", "--target", "apk"], extra_env=JENV)


def find_apk():
    d = ANDROID_BUILD / "android-build/build/outputs/apk/debug"
    apks = sorted(d.glob("*.apk"), key=lambda p: p.stat().st_mtime)
    if not apks:
        raise SystemExit(f"APK not found in {d}")
    return apks[-1]


def upload_android(*_a, **_k):  # build + install + launch
    build_android()
    run([ADB, "install", "-r", str(find_apk())])
    run([ADB, "shell", "am", "force-stop", PKG], check=False)
    run([ADB, "shell", "am", "start", "-n", ACTIVITY])


def monitor_android(*_a, **_k):  # logcat
    pid = ""
    for _ in range(20):  # wait for the process to appear
        pid = subprocess.run([ADB, "shell", "pidof", PKG],
                             capture_output=True, text=True).stdout.strip()
        if pid:
            break
        time.sleep(0.5)
    cmd = [ADB, "logcat", "-v", "time"]
    if pid:
        cmd += [f"--pid={pid.split()[0]}"]
    else:
        print("App is not running, showing unfiltered errors")
        cmd += ["*:E"]
    try:
        run(cmd)
    except (KeyboardInterrupt, subprocess.CalledProcessError):
        pass


def clean_app(*_a, **_k):
    shutil.rmtree(LINUX_BUILD if target == "linux" else ANDROID_BUILD, ignore_errors=True)


if target == "linux":
    env.AddCustomTarget("build_app", None, build_linux, title="Build Linux app")  # noqa: F821
    env.AddCustomTarget("upload_app", None, run_linux, title="Build and run Linux app")  # noqa: F821
else:
    env.AddCustomTarget("build_app", None, build_android, title="Build APK")  # noqa: F821
    env.AddCustomTarget("ut", None, upload_android, title="Install and launch APK")  # noqa: F821
    env.AddCustomTarget("monitor_app", None, monitor_android, title="adb logcat")  # noqa: F821
env.AddCustomTarget("clean_app", None, clean_app, title="Remove build dir")  # noqa: F821
