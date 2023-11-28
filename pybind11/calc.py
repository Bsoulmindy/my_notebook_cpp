import customcpp
import time

nb_iter = 10**7

worker = customcpp.NativeWorker("ous")

# C++ : 13ms
start_time = time.time()
result = worker.heavy_work(nb_iter)
print("%s ms for C++" % ((time.time() - start_time) * 1000))

# Python : 1452ms
start_time = time.time()
print("Hello! my name is %s" % "ous")
s = 0
for i in range(0, nb_iter + 1):
    if i % 2 == 0:
        s += 1
    else:
        s -= 1
result = s
print("%s ms for Python" % ((time.time() - start_time) * 1000))

print(result)
