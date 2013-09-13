#!/usr/bin/python

VERSION='0.0.1'
APPNAME='ArdourSceneManager'
top = '.'
out = 'build'

def set_options(opt):
	print '\nSetting build options & flags...'
	# We tell Waf we need a C++ compiler
	opt.tool_options('compiler_cxx')

def init(null):
  #print '\nInitializing ASM...'
  pass

def configure(ctx):
  
  ctx.check_tool ('compiler_cxx')
  
  ctx.env.CXXFLAGS = ['-pg','-g','-ldl','-Wall','-std=gnu++0x','-Wextra','-fpermissive']
  
  # Engine Depends
  ctx.check_cfg(package='jack',at_least_version='0.118',args='--cflags --libs',uselib_store='JACK')
  ctx.check_cfg(package='libxml-2.0',args='--cflags --libs',uselib_store='XML')
  ctx.check_cfg(package='gtkmm-3.0',at_least_version='3.4.0',args='--cflags --libs',uselib_store='GTKMM')

def build(ctx):
  
  # Source Files
  sourceList=['src/ArdourSceneManager.cpp',
              'src/ASMView.cpp',
              'src/JackMIDI/jackMIDI.cpp',
              'src/Scene/Scene.cpp',
              'src/Scene/ArdourControllable.cpp',
              'src/Scene/Track.cpp',
              'src/Scene/Send.cpp',
              'src/SceneParser/SceneParser.cpp',
              'src/ArdourSessionParser/ArdourSession.cpp']
  
  
  sourceDepends = 'JACK GTKMM XML'
  
  #print 'Building ASM'
  buildList = sourceList
  dependList = sourceDepends
    
  main = ctx.new_task_gen(
    features = 'cxx cprogram',
    source = buildList,
    uselib = dependList,
    target = 'ArdourSceneManager' )

  ctx.install_files('/usr/share/ardour3/midi_maps',
		 ['extras/ArdourSceneManager.map'] )

  ctx.install_as('/usr/share/asm/icon.png','extras/knob.png')

  ctx.install_files('/usr/share/applications',
		 ['extras/ArdourSceneManager.desktop'] )

