
n = int(input())
m = int(input())

StrenghtOfCastle = list(map(int,input().split()))
impOfCastle = list(map(int,input().split()))
StrenghtOfCanon = list(map(int,input().split()))

impOfCastle= [x for _,x in sorted(zip(StrenghtOfCastle,impOfCastle))]
StrenghtOfCastle.sort()
sum=0

print(StrenghtOfCastle)
print(impOfCastle)

for i in range(1,n):
    if impOfCastle[i]<impOfCastle[i-1]:
        impOfCastle[i] =impOfCastle[i-1]

for i in range(m):

    tmp = StrenghtOfCanon[i]
    low,high = 0, n-1

    while low<high:
        mid = (low+high)//2
        if StrenghtOfCastle[mid]<=tmp:
            low = mid+1
        else:
            high=mid
    if(tmp<StrenghtOfCastle[low] and low!=0):
        low=low-1

    sum+= impOfCastle[low]

print(sum)
    
