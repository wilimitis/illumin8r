env = Environment(
  CPPPATH = ['libs'],
  CXXFLAGS = ['-std=c++17'],
  FRAMEWORKS = ['GLUT', 'OpenGL']
)
env.VariantDir('build', 'src', duplicate = 0)
env.Program('build/illumin8r', source = [Glob('build/*.cpp'), Glob('build/**/*.cpp')])
