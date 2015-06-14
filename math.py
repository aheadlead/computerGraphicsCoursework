import operator
def c(n, k):
    return reduce(operator.mul, range(n-k+1, n+1), 1) / reduce(operator.mul,
            range(1, k+1), 1)

print c(33, 16)
print c(33, 17)
print c(33, 18)
exit()

for n in range(0, 50):
    for k in range(0, n+1):
        p = c(n, k)
        print "%8d" % (p),
    print

