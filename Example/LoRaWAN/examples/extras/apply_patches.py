from os.path import join, isfile

Import("env")

FRAMEWORK_DIR = env.PioPlatform().get_package_dir("framework-arduinoespressif32")
patchflag_path = join(FRAMEWORK_DIR, ".patching-done")

# patch file only if we didn't do it before
if not isfile(join(FRAMEWORK_DIR, ".patching-done")):
    original_file = join(FRAMEWORK_DIR, "variants", "pico32", "pins_arduino.h")
    patched_file = join("examples", "extras", "patches", "pico32_dragino_trackerd.patch")

    assert isfile(original_file) and isfile(patched_file)

    env.Execute("patch %s %s" % (original_file, patched_file))
    # env.Execute("touch " + patchflag_path)


    def _touch(path):
        with open(path, "w") as fp:
            fp.write("")

    env.Execute(lambda *args, **kwargs: _touch(patchflag_path))
