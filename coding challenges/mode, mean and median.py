import operator

def avgMiddle(arr):
    middle = int((len(arr) - 1) / 2)
    if len(arr) % 2 == 0:
        return (arr[middle] + arr[middle + 1]) / 2
    else: 
        return arr[middle]

def findOccurrences(arr):
    ocurrences = {}
    smallestVal = 1
    largestVal = 1

    for i in range(len(arr)):
        if arr[i] in ocurrences:
            ocurrences[arr[i]] += 1
            largestVal = ocurrences[arr[i]]
        else:
            ocurrences[arr[i]] = 1
    
    if largestVal == smallestVal:
        return arr[0]
    else:
        return int(max(ocurrences.items(), key=operator.itemgetter(1))[0])


count = int(input())
string = input()

arr = [int(x) for x in string.split(" ")]

print(round(sum(arr)/count, 1))

arr.sort()

print(avgMiddle(arr))

print(findOccurrences(arr))
