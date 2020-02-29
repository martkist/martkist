from distutils.core import setup
setup(name='sysspendfrom',
      version='1.0',
      description='Command-line utility for martkist "coin control"',
      author='Gavin Andresen',
      author_email='gavin@martkistfoundation.org',
      requires=['jsonrpc'],
      scripts=['spendfrom.py'],
      )
