import itertools as it 

xs = [1, 2, 3, 4, 5, 6, 7, 8]
pairs = list(it.combinations(xs, 2))
obj = list(it.combinations(pairs, len(xs) // 2))

x = [ps for ps in obj if len(set(it.chain.from_iterable(ps))) == len(xs)]

print(len(x))

y = [bin(obj.index(i)).replace("0b","") for i in x]

choice_indices = list(it.product(*[range(k) for k in reversed(range(1, 8, 2))]))
print(len(choice_indices))

for i,j  in zip(choice_indices, x):
    print(i,j)

    


# print(*y, sep = "\n")
