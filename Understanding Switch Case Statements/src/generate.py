if __name__ == '__main__':
  code = '''
void __switch_test(unsigned int type) {
  volatile int r; // NOTE: preventing compiler from optimizing it out

  switch(type) {
  ''';
  for i in range(0, 100000):
      code += 'case ' + str(i) + ':{ \n' + ' r=' + str(i) + ';\n break;\n}\n'
  
  code += 'default: {\n r = 11;\n break;\n}\n'
  code += '}'
  code += '}'

  print(code)
