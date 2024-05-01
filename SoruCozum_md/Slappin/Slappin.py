
theN = int(input())
mod = 10**3+7
dp=[0]*(10**3+9)



def Climb(n):

  dp[0]=2
  dp[1]=2
  dp[2]=3
 

  for i in range(3,n+1):
      dp[i]=(dp[i-2] +dp[i-3])%mod

  return dp[n]

print(Climb(theN))