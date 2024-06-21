
n = int(input())

legs=[]

for i in range(n):
    l,r=map(int,input().split()) 
    legs.append(l)
    legs.append(r)

legs.sort()

sum = 0
for i in range(0,n*2,2):
    sum+=legs[i+1]-legs[i]

print(sum)