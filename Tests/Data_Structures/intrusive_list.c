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

static void test_AddTail(RFilist_head* l, int* arr, int arr_size)
{
    int i;
    an_object* obj;

    //add the objects
    for(i = 0; i < arr_size; i++)
    {
        obj = malloc(sizeof(an_object));
        obj->num = arr[i];
        rf_ilist_add_tail(l, &obj->lh);
    }

    //iterate the list and confirm they have been added correctly
    i=0;
    rf_ilist_for_each(l, obj, lh)
    {
        EXPECT(obj->num, arr[i]);
        i++;
    }
}

static void test_Add(RFilist_head* l, int* arr, int arr_size)
{
    int i;
    an_object* obj;

    //add the objects
    for(i = 0; i < arr_size; i++)
    {
        obj = malloc(sizeof(an_object));
        obj->num = arr[i];
        rf_ilist_add(l, &obj->lh);
    }

    //iterate the list and confirm they have been added correctly
    i=arr_size-1;
    rf_ilist_for_each(l, obj, lh)
    {
        EXPECT(obj->num, arr[i]);
        i--;
    }
}


static void test_Delete(RFilist_head* l)
{
    an_object* obj, *tmp;
    rf_ilist_for_each_safe(l, obj, tmp, lh)
    {
        rf_ilist_delete_from(l, &obj->lh);
        free(obj);
    }
    EXPECT(rf_ilist_is_empty(l), true);
}

static void test_Pop(RFilist_head* l, int* arr, int arr_size)
{
    an_object* obj, *tmp;
    int i = arr_size - 1;
    rf_ilist_for_each_safe(l, obj, tmp, lh)
    {
        obj = rf_ilist_pop(l, an_object, lh);
        EXPECT(obj->num, arr[i]);
        free(obj);
        i--;
    }
    EXPECT(rf_ilist_is_empty(l), true);
}

static void test_List_Append_Prepend(RFilist_head* l1, RFilist_head* l2,
                                     RFilist_head* l3, int* arr1, int arr1_size,
                                     int* arr2, int arr2_size, int* arr3,
                                     int arr3_size)
{
    int i;
    an_object* obj;
    //populate the 3 lists
    test_AddTail(l1, arr1, arr1_size);
    test_AddTail(l2, arr2, arr2_size);
    test_AddTail(l3, arr3, arr3_size);

    rf_ilist_prepend_list(l1, l2);
    EXPECT(rf_ilist_is_empty(l2), true);
    rf_ilist_append_list(l1, l3);
    EXPECT(rf_ilist_is_empty(l3), true);

    //iterate the list and confirm the merging happened correctly
    i=0;
    rf_ilist_for_each(l1, obj, lh)
    {
        EXPECT(obj->num, merged_arr[i]);
        i++;
    }
}

int main()
{
    RFilist_head list1, list2, list3;
    DEFAULT_LIB_INIT();

    rf_ilist_head_init(&list1);

    test_AddTail(&list1, arr1, sizeof(arr1)/sizeof(int));
    EXPECT(rf_ilist_is_empty(&list1), false);
    
    test_Delete(&list1);

    test_Add(&list1, arr1, sizeof(arr1)/sizeof(int));
    test_Pop(&list1, arr1, sizeof(arr1)/sizeof(int));

    rf_ilist_head_init(&list2);
    rf_ilist_head_init(&list3);
    test_List_Append_Prepend(&list1, &list2, &list3,
                             arr1, sizeof(arr1)/sizeof(int),
                             arr2, sizeof(arr2)/sizeof(int),
                             arr3, sizeof(arr3)/sizeof(int));


    return 0;
}
