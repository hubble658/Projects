

theN,price = map(int,(input().split()))
mod = 10**9+7

coinList = list(map(int,input().split()))

sorted(coinList)
dp=[[0]*(price+1)]*(theN+1)

for i in range(1,theN+1):
    coin = coinList[i-1]
    print(dp[i])
    for k in range(1,price+1):
        if k==coin:
            dp[i][k]=(dp[i-1][k]+1)%mod
        else:
            if k-coin>=0:
                dp[i][k]=(dp[i][k-coin]+dp[i-1][k])%mod
            else:
                dp[i][k]=dp[i-1][k]

print(dp[theN][price])

