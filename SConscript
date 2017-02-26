Import('env')

env.CompileFolderDWIM('Entendre')
env.CompileFolderDWIM('graph_display')
env.IrrlichtLib()
env.SharedLibraryDir('libTestCases',dependencies=['neat','Entendre'])
env.PythonLibraryDir('pyTestCases',dependencies=['neat','Entendre'])
env.MainDir('.')
