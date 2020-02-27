import sys
import argparse
import os
from struct import pack

def do_boot(args):
    print("boot")
    big_binary=b''
    with open(args.infile, 'rb') as f:
        big_binary=f.read()
        big_binary=big_binary[2048:]
    with open(args.outfile, 'wb') as f:
       f.write(big_binary)        


def do_cbor(args):
    print("cbor")
    big_binary=b''
    with open(args.infile, 'rb') as f:
        all_line=f.readlines()
        for line in all_line:
            
            for i in range(0,len(line)>>1):  
                hexavalue=line[2*i:2*i+2];
                try:
                    byte=int(hexavalue,16)
                    big_binary +=pack("B",byte)
                except:
                    endline=True
    with open(args.outfile, 'wb') as f:
       f.write(big_binary)        

subcmds = {
        'cbor':do_cbor,
        'boot':do_boot
        }
def args():
    parser = argparse.ArgumentParser()
    subs = parser.add_subparsers(help='subcommand help', dest='subcmd')
    cbor= subs.add_parser('cbor', help='build cbor')
    cbor.add_argument("infile", help="input filename of text file" )
    cbor.add_argument("outfile", help="output filename cbor file" )
    boot = subs.add_parser('boot', help='build boot')
    boot.add_argument("infile", help="input filename of text file" )
    boot.add_argument("outfile", help="output filename cbor file" )

    args = parser.parse_args()

    if args.subcmd is None:
        print('Must specify a subcommand')
        sys.exit(1)
    subcmds[args.subcmd](args)
if __name__ == '__main__':
    args()
