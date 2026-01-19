import subprocess
import re


degrees = [1000, 5000, 10000, 20000] 
threads_list = [1, 2, 4, 8]
runs_per_experiment = 4

executable = "./poly_mult_omp"

print(f"{'Degree':<8} | {'Thr':<3} | {'Avg Serial(s)':<14} | {'Avg Parallel(s)':<14} | {'Speedup':<8}")
print("-" * 75)

for n in degrees:
    for t in threads_list:
        serial_times = []
        parallel_times = []
        
        # Εκτέλεση του πειράματος 
        for _ in range(runs_per_experiment):
            try:
                result = subprocess.run(
                    [executable, str(n), str(t)], 
                    capture_output=True, 
                    text=True,
                    check=True
                )
                output = result.stdout
                
                # Εξαγωγή χρόνων με Regex
                s_match = re.search(r"Serial Time: ([0-9.]+)", output)
                p_match = re.search(r"Parallel Time: ([0-9.]+)", output)
                
                if s_match and p_match:
                    serial_times.append(float(s_match.group(1)))
                    parallel_times.append(float(p_match.group(1)))
            except Exception as e:
                print(f"Error running {n} deg, {t} threads: {e}")

        # Υπολογισμός μέσων όρων
        if serial_times and parallel_times:
            avg_serial = sum(serial_times) / len(serial_times)
            avg_parallel = sum(parallel_times) / len(parallel_times)
            
            # Speedup
            speedup = avg_serial / avg_parallel if avg_parallel > 0 else 0
            
            print(f"{n:<8} | {t:<3} | {avg_serial:<14.6f} | {avg_parallel:<14.6f} | {speedup:<8.2f}")
        else:
            print(f"{n:<8} | {t:<3} | {'ERROR':<14} | {'ERROR':<14} | {'N/A':<8}")
