def upper_bound(arr, target):
    """
    Find the index of the smallest element in a sorted list that is greater than the target.
    If all elements are less than or equal to the target, returns the length of the list.

    :param arr: List of elements (must be sorted).
    :param target: Target value to find the upper bound for.
    :return: Index of the upper bound element.
    """
    low, high = 0, len(arr)-1
    
    while low < high:
        mid = (low + high) // 2
        if arr[mid] <= target:
            low = mid + 1
        else:
            high = mid

    return low

# Example usage:
sorted_list = [1, 3, 4, 6, 8, 10,15]


# More tests
print(sorted_list[upper_bound(sorted_list, 11)])  # Output should be 7 (index after the last element)
print(sorted_list[upper_bound(sorted_list, 19)])  # Output should be 0 (index of the first element)
print(sorted_list[upper_bound(sorted_list, 15)])  # Output should be 0 (index of the first element)
print(sorted_list[upper_bound(sorted_list, 1)])  # Output should be 0 (index of the first element)
