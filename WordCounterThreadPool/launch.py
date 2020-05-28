import subprocess
import argparse
import platform
import tempfile
import os

def call_counter(filename: str):

        stm = platform.system()
        res = None
        if (stm == "Windows"):
            res = subprocess.run([r"bin\WordCounter.exe", filename], universal_newlines=True, stdout=subprocess.PIPE)
        else:
            res = subprocess.run([r"bin/WordCounter", filename], universal_newlines=True, stdout=subprocess.PIPE)
        
        res.check_returncode()
        return res

if __name__ == "__main__":
    
    parser = argparse.ArgumentParser(description="Count the number of occurences of each word in a directory containing text files.")
    parser.add_argument("-f", "--file", default="example.conf", help="Path to configuration file. If no other options are provided, \'example.conf\' is used by default")
    parser.add_argument("-mt", "--maxthreads", help="How many additional threads can the program use. The exact number of threads is calculated as: nthread = std::hardware_concurrency() + max_threads. Can be a negative number")
    parser.add_argument("-d", "--data", help="Path to the root directory starting from which files should be indexed.")
    parser.add_argument("-oa", "--outa", help="Path to the result, sorted in lexocographical order.")
    parser.add_argument("-on", "--outn", help="Path to the result, sorted by the number of occurences.")
    parser.add_argument("-b", "--boundary", help="Upper bound on the number of elements in the queue. Must be > 1.")
    parser.add_argument("-v", "--verbose", help="Display additional debug info in output.", action="store_true")
    parser.add_argument("-o", "--output", help="Path to the file where the timings will be stored. STDOUT by default.", default="STDOUT")
    
 
    arguments = parser.parse_args()
    conf_arguments = [arguments.maxthreads, arguments.data, arguments.outa, arguments.outn, arguments.boundary]
    
    result = None
    if any(conf_arguments) and (arguments.file != "example.conf"):

        parser.error("Can't accept both a config file and config parameters.")

    elif not any(conf_arguments):

        result = call_counter(arguments.file)

    elif all(conf_arguments):

        tmp = tempfile.NamedTemporaryFile(dir=os.path.abspath("."))
        try:

            tmp.write(f"max_threads = {int(conf_arguments[0])}\n".encode())
            tmp.write(f"data_path = {conf_arguments[1]}\n".encode())
            tmp.write(f"out_A_path = {conf_arguments[2]}\n".encode())
            tmp.write(f"out_N_path = {conf_arguments[3]}\n".encode())
            tmp.write(f"boundary = {int(conf_arguments[4])}".encode())
            result = call_counter(tmp)

        finally:

            tmp.close()

    else:

        parser.error("Invalid options. Perhaps you forgot some config arguments? Use --help to learn more about config arguments.")
    
    if arguments.output == "STDOUT":

        if not arguments.verbose:
            
            print(result.stdout.split("\n")[-2])

        else:

            print(result.stdout)

    else:

        with open(arguments.output, 'w') as f:

            if not arguments.verbose:
            
                f.write(result.stdout.split("\n")[-2])

            else:

                f.writelines(result.stdout)
