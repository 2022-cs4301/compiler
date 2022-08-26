#include <iostream>
#include <vector>
#include <algorithm>
#define MAX 100
int tmp[MAX] = {};
int* brr;
void merge(int[], int, int);
void mergeSort(int[], int, int);



int main()
{

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    std::cout.tie(NULL);


    std::cout << "Welcome to CS 4301!\n";
    std::cout << "This is going to be fun!\n";


    int testnum;

    std::cout << "How many numbers you would like to put? : ";
    std::cin >> testnum;

    brr = new int[testnum];

    for (int i = 0; i < testnum; i++)
    {
        std::cin >> brr[i];
    }

    mergeSort(brr, 0, testnum - 1);

    std::cout << "Here are MergeSorted numbers : ";

    for (int i = 0; i < testnum; i++)
    {
        std::cout << brr[i] << " ";
    }


}

void merge(int arr[], int low, int mid, int high)
{
    int i = low, j = mid + 1, k = 0;
    while (i <= mid && j <= high)
    {
        if (arr[i] <= arr[j])
        {
            tmp[k] = arr[i];
            ++i, ++k;
        }
        else
        {
            tmp[k] = arr[j];
            ++j, ++k;
        }
    }

    while (i <= mid)
    {
        tmp[k] = arr[i];
        ++i, ++k;
    }

    while (j <= high)
    {
        tmp[k] = arr[j];
        ++j, ++k;
    }
    k--;

    while (k >= 0)
    {
        arr[low + k] = tmp[k];
        k--;
    }

    return;
}

void mergeSort(int arr[], int low, int high)
{
    int mid = (low + high) / 2;

    if (low < high)
    {
        mergeSort(arr, low, mid);
        mergeSort(arr, mid + 1, high);
        merge(arr, low, mid, high);
    }
    else
        return;
}

class Kim
{
private:
    int m_ID;

public:

    Kim()
    {
        m_ID = 0;
    }

    Kim(int id)
    {
        SetID(id);
    }

    void SetID(int id)
    {
        m_ID = id;
    }

    int GetID()
    {
        return m_ID;
    }

    void printThis()
    {
        std::cout << this;
    }
};