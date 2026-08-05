// The MIT License (MIT)
//
// Copyright (c) 2013 lailongwei<lailongwei@126.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <llbc.h>
using namespace llbc;

#include <vector>

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/core/objbase/Array.cpp
// @coverage-target: llbc/include/llbc/core/objbase/ArrayInl.h
// @coverage-target: llbc/src/core/objbase/Dictionary.cpp
// @coverage-target: llbc/include/llbc/core/objbase/DictionaryInl.h
// @coverage-target: llbc/src/core/objbase/DictionaryElem.cpp

namespace
{

class ValueObject final : public LLBC_Object
{
public:
    ValueObject() = default;
    explicit ValueObject(int value)
    : value(value)
    {
    }

    ~ValueObject() override
    {
        ++destructions;
    }

    LLBC_Object *Clone() const override
    {
        return new ValueObject(value);
    }

    void Serialize(LLBC_Stream &stream) const override
    {
        stream << value;
    }

    bool Deserialize(LLBC_Stream &stream) override
    {
        return stream.Read(value);
    }

    static inline int destructions = 0;
    int value = 0;
};

class ValueObjectFactory final : public LLBC_ObjectFactory
{
public:
    LLBC_Object *CreateObject() const override
    {
        return new ValueObject;
    }

    LLBC_Object *Clone() const override
    {
        return new ValueObjectFactory;
    }
};

ValueObject *MakeValue(int value)
{
    return new ValueObject(value);
}

std::vector<int> ReadArrayValues(const LLBC_Array &array)
{
    std::vector<int> values;
    for (auto it = array.Begin(); it != array.End(); ++it)
        values.push_back(static_cast<const ValueObject *>(*it)->value);

    return values;
}

std::vector<int> ReadDictionaryValues(const LLBC_Dictionary &dictionary)
{
    std::vector<int> values;
    for (auto it = dictionary.Begin(); it != dictionary.End(); ++it)
        values.push_back(static_cast<const ValueObject *>(*it)->value);

    return values;
}

} // namespace

// LLBC_Array owns strong references, supports vector-like edits/iteration, and
// can clone/serialize elements through an object factory.
TEST(ObjbaseCollectionsTest, ArrayOwnsEditsSortsClonesAndSerializesObjects)
{
    ValueObject::destructions = 0;
    ValueObjectFactory factory;
    LLBC_Array array;
    EXPECT_TRUE(array.IsEmpty());
    EXPECT_GE(array.GetCapacity(), 1);
    EXPECT_EQ(array.FirstObject(), nullptr);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);
    EXPECT_EQ(array.LastObject(), nullptr);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);

    auto *three = MakeValue(3);
    auto *one = MakeValue(1);
    auto *two = MakeValue(2);
    array.PushBack(three);
    array.PushBack(one);
    array.PushFront(two);
    three->Release();
    one->Release();
    two->Release();
    EXPECT_EQ(ReadArrayValues(array), std::vector<int>({2, 3, 1}));

    auto *four = MakeValue(4);
    ASSERT_NE(array.Insert(1, four), array.End());
    four->Release();
    EXPECT_EQ(ReadArrayValues(array), std::vector<int>({2, 4, 3, 1}));

    auto *nine = MakeValue(9);
    ASSERT_NE(array.Replace(2, nine), array.End());
    nine->Release();
    EXPECT_EQ(ReadArrayValues(array), std::vector<int>({2, 4, 9, 1}));
    EXPECT_EQ(array.ObjectAtIndex(99), nullptr);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);

    int foreachSum = 0;
    int foreachCalls = 0;
    bool stopAfterSecond = false;
    auto foreachFn = [&](LLBC_Object *object) {
        foreachSum += static_cast<ValueObject *>(object)->value;
        ++foreachCalls;
        return !stopAfterSecond || foreachCalls < 2;
    };
    array.Foreach(foreachFn);
    EXPECT_EQ(foreachSum, 16);

    stopAfterSecond = true;
    foreachCalls = 0;
    array.Foreach(foreachFn);
    EXPECT_EQ(foreachCalls, 2);

    auto ascending = [](LLBC_Object *left, LLBC_Object *right) {
        return static_cast<ValueObject *>(left)->value < static_cast<ValueObject *>(right)->value;
    };
    array.Sort(ascending);
    EXPECT_EQ(ReadArrayValues(array), std::vector<int>({1, 2, 4, 9}));
    array.Sort(array.Begin(), array.Begin(), ascending);
    EXPECT_EQ(static_cast<ValueObject *>(*array.ReverseBegin())->value, 9);
    const LLBC_Array &constArray = array;
    EXPECT_EQ(static_cast<const ValueObject *>(constArray.FirstObject())->value, 1);
    EXPECT_EQ(static_cast<const ValueObject *>(constArray.LastObject())->value, 9);
    EXPECT_EQ(static_cast<const ValueObject *>(constArray.ObjectAtIndex(2))->value, 4);
    EXPECT_EQ(static_cast<const ValueObject *>(constArray[1])->value, 2);

    LLBC_Array::IndexSet indexes {1, 3, 99};
    LLBC_Array *subset = array.ObjectsAtIndexs(indexes);
    ASSERT_NE(subset, nullptr);
    EXPECT_EQ(ReadArrayValues(*subset), std::vector<int>({2, 9}));
    subset->Release();

    array.SetObjectFactory(&factory);
    LLBC_Object *cloneObject = array.Clone();
    auto *clone = static_cast<LLBC_Array *>(cloneObject);
    ASSERT_NE(clone, nullptr);
    EXPECT_EQ(ReadArrayValues(*clone), std::vector<int>({1, 2, 4, 9}));
    clone->Release();

    LLBC_Stream stream;
    array.Serialize(stream);
    LLBC_Array noFactory;
    LLBC_Stream noFactoryStream;
    array.Serialize(noFactoryStream);
    EXPECT_FALSE(noFactory.Deserialize(noFactoryStream));
    LLBC_Array restored;
    restored.SetObjectFactory(&factory);
    ASSERT_TRUE(restored.Deserialize(stream));
    EXPECT_EQ(ReadArrayValues(restored), std::vector<int>({1, 2, 4, 9}));

    LLBC_Array operations;
    for (int value : {1, 2, 3})
    {
        auto *object = MakeValue(value);
        operations.PushBack(object);
        object->Release();
    }
    LLBC_Array additions;
    for (int value : {7, 8})
    {
        auto *object = MakeValue(value);
        additions.PushBack(object);
        object->Release();
    }
    EXPECT_NE(operations.Insert(1, additions), operations.End());
    EXPECT_NE(operations.Replace(1, 3, additions), operations.End());
    EXPECT_EQ(operations.Replace(operations.Begin(), operations.End(), operations), operations.End());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    EXPECT_EQ(operations.Insert(operations.End(), nullptr), operations.End());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    EXPECT_EQ(operations.Replace(operations.End(), nullptr), operations.End());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    operations.PushFront(operations.Begin());
    operations.PushBack(operations.Begin());
    operations.PopFront();
    operations.PopBack();
    EXPECT_NE(operations.Erase(0), operations.End());
    EXPECT_NE(operations.Erase(operations.Begin(), operations.Begin() + 1), operations.End());

    auto *duplicateObject = MakeValue(11);
    operations.PushBack(duplicateObject);
    operations.PushBack(duplicateObject);
    duplicateObject->Release();
    EXPECT_EQ(operations.Erase(duplicateObject, true), 2);

    auto *detached = MakeValue(7);
    array.PushBack(detached);
    detached->Release();
    EXPECT_EQ(array.Erase(detached, false), 0);
    EXPECT_EQ(array.GetSize(), 4);
    detached->Release();
    array.Clear();
    EXPECT_TRUE(array.IsEmpty());
}

// LLBC_Dictionary supports mixed integer/string keys and maintains a linked
// insertion order that Sort() can reorder without breaking hash lookup.
TEST(ObjbaseCollectionsTest, DictionaryOwnsMixedKeysRehashesSortsAndSerializesObjects)
{
    ValueObject::destructions = 0;
    ValueObjectFactory factory;
    LLBC_Dictionary dictionary(3);
    EXPECT_TRUE(dictionary.IsEmpty());
    EXPECT_EQ(dictionary.SetHashBucketSize(0), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_INVALID);
    EXPECT_EQ(dictionary.Insert(1, nullptr), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_INVALID);

    auto *three = MakeValue(3);
    auto *one = MakeValue(1);
    auto *two = MakeValue(2);
    ASSERT_EQ(dictionary.Insert(1, three), LLBC_OK);
    ASSERT_EQ(dictionary.Insert("one", one), LLBC_OK);
    ASSERT_EQ(dictionary.Insert(2, two), LLBC_OK);
    three->Release();
    one->Release();
    two->Release();
    auto *duplicate = MakeValue(99);
    EXPECT_EQ(dictionary.Insert(1, duplicate), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_REPEAT);
    duplicate->Release();

    ASSERT_NE(dictionary.Find(1), dictionary.End());
    ASSERT_NE(dictionary.Find("one"), dictionary.End());
    EXPECT_EQ(static_cast<const ValueObject *>(dictionary[1])->value, 3);
    EXPECT_EQ(static_cast<const ValueObject *>(dictionary["one"])->value, 1);
    EXPECT_EQ(dictionary[999], nullptr);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);

    auto *replacement = MakeValue(4);
    ASSERT_EQ(dictionary.Replace(1, replacement), LLBC_OK);
    replacement->Release();
    EXPECT_EQ(static_cast<ValueObject *>(*dictionary.Find(1))->value, 4);
    auto *stringReplacement = MakeValue(5);
    ASSERT_EQ(dictionary.Replace("one", stringReplacement), LLBC_OK);
    stringReplacement->Release();
    auto *iteratorReplacement = MakeValue(6);
    ASSERT_EQ(dictionary.Replace(dictionary.Find(2), iteratorReplacement), LLBC_OK);
    iteratorReplacement->Release();
    auto *invalidReplacement = MakeValue(99);
    EXPECT_EQ(dictionary.Replace(dictionary.End(), invalidReplacement), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_INVALID);
    invalidReplacement->Release();
    EXPECT_EQ(dictionary.Replace("missing", nullptr), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_INVALID);
    EXPECT_EQ(dictionary.SetHashBucketSize(17), LLBC_OK);
    EXPECT_EQ(static_cast<ValueObject *>(*dictionary.Find("one"))->value, 5);

    auto ascending = [](const LLBC_DictionaryElem *left, const LLBC_DictionaryElem *right) {
        return static_cast<const ValueObject *>(left->GetObject())->value <
               static_cast<const ValueObject *>(right->GetObject())->value;
    };
    LLBC_Dictionary emptyForSort;
    emptyForSort.Sort(ascending);

    LLBC_Dictionary headMoveProbe;
    for (int value : {2, 1, 3})
    {
        auto *object = MakeValue(value);
        ASSERT_EQ(headMoveProbe.Insert(value, object), LLBC_OK);
        object->Release();
    }
    headMoveProbe.Sort(ascending);
    EXPECT_EQ(ReadDictionaryValues(headMoveProbe), std::vector<int>({1, 2, 3}));

    dictionary.Sort(ascending);
    EXPECT_EQ(ReadDictionaryValues(dictionary), std::vector<int>({4, 5, 6}));
    EXPECT_EQ(static_cast<const ValueObject *>(*dictionary.ReverseBegin())->value, 6);
    const LLBC_Dictionary &constDictionary = dictionary;
    EXPECT_EQ(static_cast<const ValueObject *>(*constDictionary.ReverseBegin())->value, 6);
    int foreachSum = 0;
    auto foreachFn = [&](LLBC_DictionaryElem *elem) {
        foreachSum += static_cast<ValueObject *>(elem->GetObject())->value;
    };
    dictionary.Foreach(foreachFn);
    EXPECT_EQ(foreachSum, 15);

    dictionary.SetObjectFactory(&factory);
    LLBC_Object *cloneObject = dictionary.Clone();
    auto *clone = static_cast<LLBC_Dictionary *>(cloneObject);
    ASSERT_NE(clone, nullptr);
    EXPECT_EQ(ReadDictionaryValues(*clone), std::vector<int>({4, 5, 6}));
    clone->Release();

    LLBC_Stream stream;
    dictionary.Serialize(stream);
    LLBC_Dictionary noFactory;
    LLBC_Stream noFactoryStream;
    dictionary.Serialize(noFactoryStream);
    EXPECT_FALSE(noFactory.Deserialize(noFactoryStream));
    LLBC_Dictionary restored;
    restored.SetObjectFactory(&factory);
    ASSERT_TRUE(restored.Deserialize(stream));
    EXPECT_EQ(ReadDictionaryValues(restored), std::vector<int>({4, 5, 6}));

    ASSERT_EQ(dictionary.Erase("one"), LLBC_OK);
    EXPECT_EQ(dictionary.Erase("one"), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);
    ASSERT_EQ(dictionary.Erase(2), LLBC_OK);
    dictionary.Erase(static_cast<LLBC_Object *>(*dictionary.Find(1)));
    EXPECT_EQ(dictionary.GetSize(), 0);
    dictionary.Clear();
    EXPECT_TRUE(dictionary.IsEmpty());
}

// Insertion sort must move a new minimum to the list head while preserving the
// tail and linked iteration order. Empty dictionaries are a supported no-op.
TEST(ObjbaseCollectionsTest, DictionarySortReordersHeadAndNoopsForSmallDictionaries)
{
    auto ascending = [](const LLBC_DictionaryElem *left, const LLBC_DictionaryElem *right) {
        return static_cast<const ValueObject *>(left->GetObject())->value <
               static_cast<const ValueObject *>(right->GetObject())->value;
    };

    LLBC_Dictionary empty;
    empty.Sort(ascending);
    EXPECT_TRUE(empty.IsEmpty());

    LLBC_Dictionary dictionary;
    for (int value : {2, 1, 3})
    {
        auto *object = MakeValue(value);
        ASSERT_EQ(dictionary.Insert(value, object), LLBC_OK);
        object->Release();
    }

    dictionary.Sort(ascending);
    EXPECT_EQ(ReadDictionaryValues(dictionary), std::vector<int>({1, 2, 3}));
    EXPECT_EQ(static_cast<const ValueObject *>(*dictionary.ReverseBegin())->value, 3);
}

// String-key validation and linked-list removal must preserve ownership.
// Deserialization must handle the documented zero-bucket empty payload without
// leaking an invalid-argument error, while incomplete object payloads remain
// transactional and leave the destination empty.
TEST(ObjbaseCollectionsTest, DictionaryValidatesStringKeysAndRejectsIncompletePayloads)
{
    ValueObject::destructions = 0;

    LLBC_Dictionary stringKeys(1);
    EXPECT_EQ(stringKeys.Insert("first", nullptr), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_INVALID);
    auto *first = MakeValue(1);
    ASSERT_EQ(stringKeys.Insert("first", first), LLBC_OK);
    first->Release();
    auto *duplicate = MakeValue(2);
    EXPECT_EQ(stringKeys.Insert("first", duplicate), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_REPEAT);
    duplicate->Release();

    EXPECT_EQ(stringKeys.Replace(42, nullptr), LLBC_FAILED);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_INVALID);
    auto *second = MakeValue(2);
    ASSERT_EQ(stringKeys.Insert("second", second), LLBC_OK);
    second->Release();
    auto *tailObject = static_cast<ValueObject *>(*stringKeys.Find("second"));
    stringKeys.Erase(tailObject);
    EXPECT_EQ(stringKeys.GetSize(), 1u);

    ValueObjectFactory factory;
    LLBC_Dictionary restored;
    restored.SetObjectFactory(&factory);
    auto *stale = MakeValue(77);
    ASSERT_EQ(restored.Insert("stale", stale), LLBC_OK);
    stale->Release();

    LLBC_Stream zeroBucketPayload;
    zeroBucketPayload << static_cast<uint32>(0);
    LLBC_SetLastError(LLBC_ERROR_SUCCESS);
    EXPECT_TRUE(restored.Deserialize(zeroBucketPayload));
    EXPECT_TRUE(restored.IsEmpty());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_SUCCESS);

    LLBC_Stream incompleteIntPayload;
    incompleteIntPayload << static_cast<uint32>(1)
                         << static_cast<uint32>(1)
                         << static_cast<uint8>(1)
                         << 7;
    EXPECT_FALSE(restored.Deserialize(incompleteIntPayload));
    EXPECT_TRUE(restored.IsEmpty());

    LLBC_Stream incompleteStringPayload;
    incompleteStringPayload << static_cast<uint32>(1)
                            << static_cast<uint32>(1)
                            << static_cast<uint8>(0)
                            << LLBC_String("missing-object");
    EXPECT_FALSE(restored.Deserialize(incompleteStringPayload));
    EXPECT_TRUE(restored.IsEmpty());
}

// Integer keys may be negative (for example, protocol sentinel IDs). Their
// bucket index must be normalized consistently for insertion and lookup rather
// than indexing the hash table with a negative remainder.
TEST(ObjbaseCollectionsTest, DictionarySupportsNegativeIntegerKeysAndMissingKeyReplacement)
{
    ValueObject::destructions = 0;
    LLBC_Dictionary dictionary(3);

    auto *minusOne = MakeValue(1);
    auto *minusFour = MakeValue(4);
    ASSERT_EQ(dictionary.Insert(-1, minusOne), LLBC_OK);
    ASSERT_EQ(dictionary.Insert(-4, minusFour), LLBC_OK);
    minusOne->Release();
    minusFour->Release();

    ASSERT_NE(dictionary.Find(-1), dictionary.End());
    ASSERT_NE(dictionary.Find(-4), dictionary.End());
    EXPECT_EQ(static_cast<const ValueObject *>(dictionary[-1])->value, 1);
    EXPECT_EQ(static_cast<const ValueObject *>(dictionary[-4])->value, 4);

    const LLBC_Dictionary &constDictionary = dictionary;
    EXPECT_EQ(constDictionary["missing"], nullptr);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);
    EXPECT_NE(dictionary.ReverseBegin(), dictionary.ReverseEnd());
    EXPECT_NE(constDictionary.ReverseBegin(), constDictionary.ReverseEnd());

    auto *newIntKey = MakeValue(7);
    ASSERT_EQ(dictionary.Replace(-7, newIntKey), LLBC_OK);
    newIntKey->Release();
    EXPECT_EQ(static_cast<const ValueObject *>(dictionary[-7])->value, 7);

    auto *newStringKey = MakeValue(8);
    ASSERT_EQ(dictionary.Replace("new-key", newStringKey), LLBC_OK);
    newStringKey->Release();
    EXPECT_EQ(static_cast<const ValueObject *>(dictionary["new-key"])->value, 8);

    ASSERT_EQ(dictionary.Erase(-1), LLBC_OK);
    ASSERT_EQ(dictionary.Erase(-4), LLBC_OK);
    ASSERT_EQ(dictionary.Erase(-7), LLBC_OK);
    ASSERT_EQ(dictionary.Erase("new-key"), LLBC_OK);
    EXPECT_TRUE(dictionary.IsEmpty());

    LLBC_Dictionary empty;
    empty.Erase(static_cast<LLBC_Object *>(nullptr));
    EXPECT_TRUE(empty.IsEmpty());
}

// DictionaryElem combines the dictionary's insertion-order links with its hash
// bucket chain. Exercise direct element ownership, both key representations,
// collision insertion, and unlinking from the head/middle/tail positions.
TEST(ObjbaseCollectionsTest, DictionaryElementsMaintainKeysLinksBucketsAndOwnership)
{
    ValueObject::destructions = 0;
    auto *one = MakeValue(1);
    auto *two = MakeValue(2);
    auto *three = MakeValue(3);

    {
        LLBC_DictionaryElem intElement(1, one);
        LLBC_DictionaryElem stringElement("one", two);
        LLBC_DictionaryElem secondIntElement(2, three);
        one->Release();
        two->Release();
        three->Release();

        EXPECT_TRUE(intElement.IsIntKey());
        EXPECT_FALSE(intElement.IsStrKey());
        EXPECT_EQ(intElement.GetIntKey(), 1);
        EXPECT_TRUE(intElement.GetStrKey().empty());
        EXPECT_TRUE(stringElement.IsStrKey());
        EXPECT_FALSE(stringElement.IsIntKey());
        EXPECT_EQ(stringElement.GetStrKey(), "one");
        EXPECT_EQ(stringElement.GetIntKey(), 0);

        EXPECT_EQ(static_cast<ValueObject *>(intElement.GetObject())->value, 1);
        EXPECT_EQ(static_cast<ValueObject *>(*stringElement)->value, 2);
        const LLBC_DictionaryElem &constElement = secondIntElement;
        EXPECT_EQ(static_cast<const ValueObject *>(constElement.GetObject())->value, 3);
        EXPECT_EQ(static_cast<const ValueObject *>(*constElement)->value, 3);

        intElement.SetElemPrev(&stringElement);
        intElement.SetElemNext(&secondIntElement);
        EXPECT_EQ(intElement.GetElemPrev(), &stringElement);
        EXPECT_EQ(intElement.GetElemNext(), &secondIntElement);
        EXPECT_EQ(constElement.GetElemPrev(), nullptr);
        EXPECT_EQ(constElement.GetElemNext(), nullptr);

        LLBC_DictionaryElem *buckets[1] = {nullptr};
        intElement.Hash(buckets, 1);
        stringElement.Hash(buckets, 1);
        secondIntElement.Hash(buckets, 1);
        EXPECT_EQ(intElement.GetBucket(), buckets);
        EXPECT_EQ(intElement.GetBucketSize(), 1u);
        EXPECT_EQ(intElement.GetHashValue(), 0u);
        EXPECT_EQ(stringElement.GetHashValue(), 0u);
        EXPECT_EQ(secondIntElement.GetHashValue(), 0u);
        EXPECT_EQ(buckets[0], &secondIntElement);
        EXPECT_EQ(secondIntElement.GetBucketElemPrev(), nullptr);
        EXPECT_EQ(secondIntElement.GetBucketElemNext(), &stringElement);
        EXPECT_EQ(stringElement.GetBucketElemPrev(), &secondIntElement);
        EXPECT_EQ(stringElement.GetBucketElemNext(), &intElement);
        EXPECT_EQ(intElement.GetBucketElemPrev(), &stringElement);
        EXPECT_EQ(intElement.GetBucketElemNext(), nullptr);

        const LLBC_DictionaryElem &constStringElement = stringElement;
        EXPECT_EQ(constStringElement.GetBucketElemPrev(), &secondIntElement);
        EXPECT_EQ(constStringElement.GetBucketElemNext(), &intElement);

        stringElement.CancelHash();
        EXPECT_EQ(buckets[0], &secondIntElement);
        EXPECT_EQ(secondIntElement.GetBucketElemNext(), &intElement);
        EXPECT_EQ(intElement.GetBucketElemPrev(), &secondIntElement);

        secondIntElement.CancelHash();
        EXPECT_EQ(buckets[0], &intElement);
        EXPECT_EQ(intElement.GetBucketElemPrev(), nullptr);

        intElement.CancelHash();
        EXPECT_EQ(buckets[0], nullptr);
    }

    EXPECT_EQ(ValueObject::destructions, 3);
}

// Array overloads share a small vector-like core but must preserve ownership,
// iterator bounds, and transactional deserialization behavior on malformed
// streams. Cover the convenience overloads and defensive branches directly.
TEST(ObjbaseCollectionsTest, ArrayValidatesBoundsGrowsForRangesAndRejectsIncompleteData)
{
    ValueObject::destructions = 0;
    ValueObjectFactory factory;
    LLBC_Array array;
    for (int value : {1, 2})
    {
        auto *object = MakeValue(value);
        array.PushBack(object);
        object->Release();
    }

    EXPECT_EQ(static_cast<ValueObject *>(array.FirstObject())->value, 1);
    EXPECT_EQ(static_cast<ValueObject *>(array.LastObject())->value, 2);
    EXPECT_EQ(array.Erase(static_cast<LLBC_Object *>(nullptr), true), 0);
    EXPECT_EQ(array.Erase(array.End()), array.End());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    EXPECT_EQ(array.Erase(array.Begin(), array.Begin()), array.End());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);

    auto *invalid = MakeValue(99);
    EXPECT_EQ(array.Insert(array.End() + 1, invalid), array.End());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    EXPECT_EQ(array.Replace(array.End(), invalid), array.End());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    invalid->Release();

    const LLBC_Array &constArray = array;
    EXPECT_EQ(static_cast<const ValueObject *>(constArray.FirstObject())->value, 1);
    EXPECT_EQ(static_cast<const ValueObject *>(constArray.LastObject())->value, 2);
    EXPECT_EQ(constArray.ObjectAtIndex(99), nullptr);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    EXPECT_EQ(static_cast<const ValueObject *>(constArray[0])->value, 1);
    EXPECT_EQ(static_cast<ValueObject *>(array[0])->value, 1);
    EXPECT_NE(constArray.ReverseBegin(), constArray.ReverseEnd());
    EXPECT_NE(array.ReverseBegin(), array.ReverseEnd());

    const LLBC_Array emptyArray;
    EXPECT_EQ(emptyArray.FirstObject(), nullptr);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);
    EXPECT_EQ(emptyArray.LastObject(), nullptr);
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_NOT_FOUND);

    LLBC_Array additions;
    for (int value : {7, 8, 9})
    {
        auto *object = MakeValue(value);
        additions.PushBack(object);
        object->Release();
    }

    ASSERT_NE(array.Insert(1, additions), array.End());
    EXPECT_GE(array.GetCapacity(), 5);
    EXPECT_EQ(ReadArrayValues(array), std::vector<int>({1, 7, 8, 9, 2}));
    ASSERT_NE(array.Replace(1, 4, additions), array.End());
    EXPECT_EQ(ReadArrayValues(array), std::vector<int>({1, 7, 8, 9, 2}));
    ASSERT_NE(array.Erase(1, 4), array.End());
    EXPECT_EQ(ReadArrayValues(array), std::vector<int>({1, 2}));
    EXPECT_EQ(array.Replace(array.End(), array.End() + 1, additions), array.End());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);
    EXPECT_EQ(array.Erase(array.End(), array.End() + 1), array.End());
    EXPECT_EQ(LLBC_GetLastError(), LLBC_ERROR_ARG);

    LLBC_Array iteratorInsert;
    for (int value : {1, 2})
    {
        auto *object = MakeValue(value);
        iteratorInsert.PushBack(object);
        object->Release();
    }
    ASSERT_NE(iteratorInsert.Insert(iteratorInsert.Begin() + 1, additions), iteratorInsert.End());
    EXPECT_EQ(ReadArrayValues(iteratorInsert), std::vector<int>({1, 7, 8, 9, 2}));

    LLBC_Array restored;
    restored.SetObjectFactory(&factory);
    LLBC_Stream zeroStream;
    zeroStream << static_cast<uint32>(0);
    EXPECT_TRUE(restored.Deserialize(zeroStream));
    EXPECT_TRUE(restored.IsEmpty());

    auto *stale = MakeValue(77);
    restored.PushBack(stale);
    stale->Release();
    LLBC_Stream incompleteStream;
    incompleteStream << static_cast<uint32>(1);
    EXPECT_FALSE(restored.Deserialize(incompleteStream));
    EXPECT_TRUE(restored.IsEmpty());
}
