#!/usr/bin/env python
import sys
import subprocess

if __name__ == "__main__":
    # execute only if run as a script

    # cleanup files
    subprocess.call(["rm", "-f", "G* tags flist.out"])

    dirs = [
        './CMS_PLATFORM_SERVICES', 
        './CMS_SYSTEM_INFRASTRUCTURE',
        './CMS_SYSTEM_RESOURCE',
        './DARWIN_PLATFORM', 
        './CMS_MEDIA_SERVICES', 
        './CMS_INFORMATION_SERVICES',
        './COMMON_MIDDLEWARE',
        './THIRD_PARTY_LIBRARIES/BSKYB_JTH/build/applications/Picasso/picasso/Picasso/src/java/picasso',
        './THIRD_PARTY_LIBRARIES/BSKYB_JTH/build/applications/Picasso/picasso/jpa/modules/jpa-api/src/java',
        './THIRD_PARTY_LIBRARIES/BSKYB_JTH/build/applications/Picasso/picasso/jpa/modules/jpa-impl/src/java',
        './THIRD_PARTY_LIBRARIES/BSKYB_JTH/build/applications/Picasso/picasso/jpa/modules/jpa-impl/src/c',
        './THIRD_PARTY_LIBRARIES/CRAFTWORK_LAE/lae',
        './FUSIONOS_2/UTILITIES_HELPER',
        './XTV_High_Level_VOB',
        './FUSIONOS'
        ]

    listfile = 'flist.out'
    logfile = open(listfile, 'w+')

    # find {to be replaced} -type d \( -path '*/build' -o -path '*/mock' \) -prune -o -print
    # subprocess.call(["find", ".", "-type d \( -path '*/build' -o -path '*/mock' \)", "-prune", "-o", "-print"])
    command = ["find", ".", "-type", "d", 
        "(", 
        "-path",  "*/build", "-o", 
        "-path", "*/mock",  "-o",
        "-path", "*/lib",  "-o",
        "-path", "*/tools",  "-o",
        "-path", "*/VQE_SRC",  "-o",
        "-path", "*/AVCU",  "-o",
        "-path", "*/Interface",  "-o",
        "-path", "*/doc",  "-o",
        "-path", "*/docs",  "-o",
        "-path", "*/openssl",  "-o",
        "-path", "*/FREETYPE2",  "-o",
        "-path", "*/iscdhcp",  "-o",
        "-path", "*/unit_test",  "-o",
        "-path", "*/cunitTest",  "-o",
        "-path", "*/test",  "-o",
        "-path", "*/test2",
        ")", "-prune", "-o", "-type", "f", "-print"]

    for e in dirs:
      command[1] = e
      subprocess.call(command, stdout=logfile)

    logfile.close()

    sys.stdout.write("building ctags for %s ...\n" % listfile)
    subprocess.call(["ctags", "-L", listfile])

    sys.stdout.write("building gtags for %s ...\n" % listfile)
    subprocess.call(["gtags", "--skip-unreadable", "-f", listfile])

    sys.stdout.write("done\n")
