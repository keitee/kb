# 6.1. Create the mybuild class
# Let’s create a different build function and share it.
# We can do this by creating a class in the tutorial layer.
# Therefore we create a new file called meta-tutorial/classes/mybuild.bbclass.
# 
# As in base.class, we add a build task. It is again a simple shell function.
# mybuild_do_ prefix is for following the conventions, classname_do_functionname
# for a task in a class.
# 
# EXPORT_FUNCTIONS makes the build function available to users of this class.
# If we did not have this line it could not override the build function from
# base for.
# 

addtask build
mybuild_do_build () {

  echo "running mybuild_do_build."

}

EXPORT_FUNCTIONS do_build
