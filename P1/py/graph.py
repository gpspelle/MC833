import scipy as sc
import sys
import argparse
import matplotlib
matplotlib.use('Agg')
from matplotlib import pyplot as plt
import numpy as np
from datetime import datetime, date, time

print("***********************************************************",
            file=sys.stderr)
print("             MC833 - UNICAMP 2019", file=sys.stderr)
print("***********************************************************",
        file=sys.stderr)

argp = argparse.ArgumentParser(description='Plot gaussians')
argp.add_argument("-input_cli", dest='cli', type=str, nargs=1,
    help='Usage: -input <path to input>', 
    required=True)
argp.add_argument("-input_serv", dest='serv', type=str, nargs=1,
    help='Usage: -input_serv <path to input>', 
    required=True)

try:
    args = argp.parse_args()
except:
    argp.print_help(sys.stderr)
    exit(1)

input = open(args.cli[0])
input_serv = open(args.serv[0])

for (cli, serv) in zip(input, input_serv):    
    d = date(2019, 7, 7)
    t_cli = cli.split(";");
    t_cli.pop()
    t_serv = serv.split(";")
    t_serv.pop()

    hora_min_sec1, frac_sec1 = t_cli[0].split(".")
    hora_min_sec2, frac_sec2 = t_cli[1].split(".")

    frac_sec1 = frac_sec1[:-3]
    frac_sec2 = frac_sec2[:-3]

    frac_sec1 = int(frac_sec1)
    frac_sec2 = int(frac_sec2)

    hora1, min1, sec1 = hora_min_sec1.split(":")
    hora2, min2, sec2 = hora_min_sec2.split(":")

    sec1 = int(sec1)
    sec2 = int(sec2)

    min1 = int(min1)
    min2 = int(min2)

    hora1 = int(hora1)
    hora2 = int(hora2)

    time1 = time(hora1, min1, sec1, frac_sec1)
    time2 = time(hora2, min2, sec2, frac_sec2)

    d1 = datetime.combine(d, time1)
    d2 = datetime.combine(d, time2)

    client_time = (d2-d1).total_seconds()

    hora_min_sec1, frac_sec1 = t_serv[0].split(".")
    hora_min_sec2, frac_sec2 = t_serv[1].split(".")

    frac_sec1 = frac_sec1[:-3]
    frac_sec2 = frac_sec2[:-3]

    frac_sec1 = int(frac_sec1)
    frac_sec2 = int(frac_sec2)

    hora1, min1, sec1 = hora_min_sec1.split(":")
    hora2, min2, sec2 = hora_min_sec2.split(":")

    sec1 = int(sec1)
    sec2 = int(sec2)

    min1 = int(min1)
    min2 = int(min2)

    hora1 = int(hora1)
    hora2 = int(hora2)

    time1 = time(hora1, min1, sec1, frac_sec1)
    time2 = time(hora2, min2, sec2, frac_sec2)

    d1 = datetime.combine(d, time1)
    d2 = datetime.combine(d, time2)
    
    server_time = (d2-d1).total_seconds()

    cliserver = str(client_time - server_time).replace('.', ',')
    client_time = str(client_time).replace('.', ',')
    server_time = str(server_time).replace('.', ',')

    print("{};{};{}".format(client_time, server_time, cliserver))



