#!/usr/bin/python

'''
	There are several functions that waf will call while building.
	The main one is >  build
	the other functions help to configure your build to link to the
	right libraries, or to install in the right place. Even C++ 
	optimizations and cross compilation can be achieved using Waf.
'''

def set_options(opt):
	print '\nSetting build options & flags...'
	# We tell Waf we need a C++ compiler
	opt.tool_options('compiler_cxx')

def init():
	# I've never used this function much.. for basic use its not needed.
	print 'Initializing WAF build system...'
	

def configure(conf):
	print 'Configuring the build enviroment...'
	# Make Waf check to see if we have a C++ compiler
	conf.check_tool('compiler_cxx')
	# Make Waf check to see if we have the Gtkmm development package,
	# and if we do, we store the library (gtkmm) in the "uselib_store" variable.
	# Later on, were going to use the "uselib_store" to add the library to our executable
	conf.check_cfg	(package='gtkmm-3.0',at_least_version='3.0.0',args='--cflags --libs',uselib_store='GTKMM')
	#check for jack
	conf.check_cfg	(package='jack',at_least_version='0.1.1',args='--cflags --libs',uselib_store='JACK')
	#check for libxml
	conf.check_cfg	(package='libxml-2.0',at_least_version='2.7.8',args='--cflags --libs',uselib_store='XML')

def build(bld):
	print 'Building the sources to objects...'
	# Here we add a "task" to our build. Each .cpp file will need its own
	# task. There are advanced ways of doing tasks, but were keeping it simple.
	bld.new_task_gen(
		# features  = "what" were compiling(C++,or cxx), and how to link it (cprogam)
		features	= 'cxx cprogram',
		# source    = which source file to compile
		source		= 'src/ArdourSceneManager.cpp',
		# includes  = where our included header files are
		includes	= '/usr/include',
		# uselib    = which libraries to link to. See above where we added Gtkmm to the store
		uselib		= 'GTKMM',
		# target    = what to call the output file.
		target		= 'ArdourSceneManager'	)

def shutdown():
	# this function can be used to copy files to the build directory.
	# its a little advanced, but if your using Images or other files
	# in your program, its worth knowing.
	print('shutting down')

