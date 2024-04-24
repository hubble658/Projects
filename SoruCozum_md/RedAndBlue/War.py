n = int(input())
m = int(input())

StrenghtOfCastle = list(map(int,input().split()))
impOfCastle = list(map(int,input().split()))
StrenghtOfCanon = list(map(int,input().split()))

impOfCastle= [x for _,x in sorted(zip(StrenghtOfCastle,impOfCastle))]
StrenghtOfCastle.sort()

sum=0

castle = {}

for i in range(n):
    #print(impOfCastle[i],StrenghtOfCastle[i])
    if impOfCastle[i]>castle.get(StrenghtOfCastle[i],-1):
        castle.update({StrenghtOfCastle[i]:impOfCastle[i]})
for i in range(m):

    tmp = StrenghtOfCanon[i]
    low,high = 0, n-1

    while low<high:
        mid = (low+high)//2
        if StrenghtOfCastle[mid]<=tmp:
            low = mid+1
        else:
            high=mid
    if(tmp<StrenghtOfCastle[mid] and mid!=0):
        mid=mid-1

    sum+= castle.get(StrenghtOfCastle[mid],-1)

print(sum)