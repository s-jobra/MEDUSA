PROB = 0.5

import random
from argparse import ArgumentParser

def random_oracle(n_reg2, n_gate_oracle):
    oracle_data = []
    for ith_gate in range(n_gate_oracle):
        target_qubit = random.randint(0, n_reg2 - 1)

        pos_ctrl = []
        neg_ctrl = []
        for ith_qubit in range(n_reg2):
            if ith_qubit == target_qubit:
                continue
            if random.random() < PROB:  
                if random.randint(0, 1) == 1:
                    pos_ctrl.append(ith_qubit)
                else:
                    neg_ctrl.append(ith_qubit)
            
        oracle_data.append((pos_ctrl, neg_ctrl, target_qubit))
    return oracle_data

def get_oracle_string(ctrl_qubit, oracle_data, n_reg1):
    oracle_string = ''
    for (pos_ctrl, neg_ctrl, target_qubit) in oracle_data:
        for ith_qubit in neg_ctrl:
            oracle_string += '  x q[%d];\n' % (n_reg1 + ith_qubit)

        oracle_string += '  mcx q[%d], %s;\n' % (ctrl_qubit, ', '.join(map(lambda ith_qubit:'q[%d]'%(n_reg1 + ith_qubit), pos_ctrl + neg_ctrl + [target_qubit])))
                                                  
        for ith_qubit in neg_ctrl:
            oracle_string += '  x q[%d];\n' % (n_reg1 + ith_qubit)
    return oracle_string


if __name__ == '__main__':
    parser = ArgumentParser()
    parser.add_argument("file_name",        help="name of output file")
    parser.add_argument("n_reg1",           help="number of qubits in 1st register")
    parser.add_argument("n_reg2",           help="number of qubits in 2nd register")
    parser.add_argument("n_gate_oracle",    help="number of Toffoli gates in random oracle")
    parser.add_argument("seed",             help="random seed")
    args = parser.parse_args()
    
    file_name = args.file_name
    n_reg1 = int(args.n_reg1)
    n_reg2 = int(args.n_reg2)
    n_gate_oracle = int(args.n_gate_oracle)
    seed = int(args.seed)
    
    random.seed(seed)    
    oracle_data = random_oracle(n_reg2, n_gate_oracle)
    
    with open(file_name, 'w') as file:
        file.write('OPENQASM 2.0;\n')
        file.write('include "qelib1.inc";\n')
        file.write('qreg q[%d];\n\n' % (n_reg1 + n_reg2))
        for ith_qubit in range(n_reg1):
            file.write('h q[%d];\n' % ith_qubit)        
        file.write('x q[%d];\n' % (n_reg1 + n_reg2 - 1))

        # double "for" loop here; one unrolled and one with explicit *for* loops
        for i in range(n_reg1):
            file.write("for j in [0:%d:1] {\n" % (2**i - 1))        
            file.write(get_oracle_string(i, oracle_data, n_reg1))
            file.write("}\n")

        
