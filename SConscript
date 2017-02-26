Import('env')

env.CompileFolderDWIM('Entendre')
env.IrrlichtLib()
env.SharedLibraryDir('libTestCases',dependencies=['neat','Entendre'])
env.PythonLibraryDir('pyTestCases',dependencies=['neat','Entendre'])
env.MainDir('.')
