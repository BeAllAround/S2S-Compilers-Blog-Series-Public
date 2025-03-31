
def gen_static_array():
    out = 'static int array[] = {'
    for i in range(0, 10000):
        out += str(i) + ','
    out += '};'
    print(out)

def generate_switch():
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

if __name__ == '__main__':
    # gen_static_array()
    generate_switch()
