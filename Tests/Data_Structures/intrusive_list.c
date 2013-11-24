#include <RFstring.h>
#include <RFintrusive_list.h>
#include <refu.h>

#include <stdlib.h>

#include "../refu_tests.h"


int arr1[] = {3, 4, 6, 5, 1, 2, 1};
int arr2[] = {15, 235, 234, 234};
int arr3[] = {-12, 873, -123, 0};

int merged_arr[] = {15, 235, 234, 234, 3, 4, 6, 5, 1, 2, 1, -12, 873, -123, 0};
typedef struct an_object
{
    int num;
    RF_ILNode lh;
}an_object;

static void test_AddTail(RF_ILHead* l, int* arr, int arr_size)
{
    int i;
    an_object* obj;

    //add the objects
    for(i = 0; i < arr_size; i++)
    {
        obj = malloc(sizeof(an_object));
        obj->num = arr[i];
        rfIList_AddTail(l, &obj->lh);
    }

    //iterate the list and confirm they have been added correctly
    i=0;
    rfIList_ForEach(l, obj, lh)
    {
        EXPECT(obj->num, arr[i]);
        i++;
    }
}

static void test_Add(RF_ILHead* l, int* arr, int arr_size)
{
    int i;
    an_object* obj;

    //add the objects
    for(i = 0; i < arr_size; i++)
    {
        obj = malloc(sizeof(an_object));
        obj->num = arr[i];
        rfIList_Add(l, &obj->lh);
    }

    //iterate the list and confirm they have been added correctly
    i=arr_size-1;
    rfIList_ForEach(l, obj, lh)
    {
        EXPECT(obj->num, arr[i]);
        i--;
    }
}


static void test_Delete(RF_ILHead* l)
{
    an_object* obj, *tmp;
    rfIList_ForEach_safe(l, obj, tmp, lh)
    {
        rfIList_DeleteFrom(l, &obj->lh);
        free(obj);
    }
    EXPECT(rfIList_IsEmpty(l), true);
}

static void test_Pop(RF_ILHead* l, int* arr, int arr_size)
{
    an_object* obj, *tmp;
    int i = arr_size - 1;
    rfIList_ForEach_safe(l, obj, tmp, lh)
    {
        obj = rfIList_Pop(l, an_object, lh);
        EXPECT(obj->num, arr[i]);
        free(obj);
        i--;
    }
    EXPECT(rfIList_IsEmpty(l), true);
}

static void test_List_Append_Prepend(RF_ILHead* l1, RF_ILHead* l2,
                                     RF_ILHead* l3, int* arr1, int arr1_size,
                                     int* arr2, int arr2_size, int* arr3,
                                     int arr3_size)
{
    int i;
    an_object* obj;
    //populate the 3 lists
    test_AddTail(l1, arr1, arr1_size);
    test_AddTail(l2, arr2, arr2_size);
    test_AddTail(l3, arr3, arr3_size);

    rfIList_PrependList(l1, l2);
    EXPECT(rfIList_IsEmpty(l2), true);
    rfIList_AppendList(l1, l3);
    EXPECT(rfIList_IsEmpty(l3), true);

    //iterate the list and confirm the merging happened correctly
    i=0;
    rfIList_ForEach(l1, obj, lh)
    {
        EXPECT(obj->num, merged_arr[i]);
        i++;
    }
}

int main()
{
    RF_ILHead list1, list2, list3;
    rfInit();
    rfIList_HeadInit(&list1);

    test_AddTail(&list1, arr1, sizeof(arr1)/sizeof(int));
    EXPECT(rfIList_IsEmpty(&list1), false);
    
    test_Delete(&list1);

    test_Add(&list1, arr1, sizeof(arr1)/sizeof(int));
    test_Pop(&list1, arr1, sizeof(arr1)/sizeof(int));

    rfIList_HeadInit(&list2);
    rfIList_HeadInit(&list3);
    test_List_Append_Prepend(&list1, &list2, &list3,
                             arr1, sizeof(arr1)/sizeof(int),
                             arr2, sizeof(arr2)/sizeof(int),
                             arr3, sizeof(arr3)/sizeof(int));


    return 0;
}
