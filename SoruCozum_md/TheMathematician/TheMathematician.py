n,slope = map(int,(input().split()))

liste1 = list(map(int,(input().split())))

count = 0
l = 0
while l<n:

    if (liste1[l+1]-liste1[l])/1> slope:
        break

    r = l+1
    while r<n:
        tempSlope = (liste1[r]-liste1[l])/(r-l)
        if tempSlope==slope:
            count += 1
        elif tempSlope > slope:
            break
        r+=1
    l += 1
    