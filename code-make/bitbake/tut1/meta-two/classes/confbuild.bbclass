# 8.1. Class inheritance
#
# To realize our configure/build chain we create a class that inherits the
# mybuild class and simply adds a configure task as a dependency of the build
# task.
# 
# We create this as another class, which we will then use to demonstrate a class
# and a recipe which make use of inheritance.

# use the mybuild class as a base
#
# 6.1. Create the mybuild class
# Let’s create a different build function and share it.
# We can do this by creating a class in the tutorial layer.
# Therefore we create a new file called meta-tutorial/classes/mybuild.bbclass.
# 
# meta-tutorial/classes/mybuild.bbclass

inherit mybuild

# create the new function
confbuild_do_configure() {
  echo "running configbuild_do_configure."
}

# define the order of the functions, configure before build
addtask do_configure before do_build

# export the function so that it becomes available
EXPORT_FUNCTIONS do_configure

