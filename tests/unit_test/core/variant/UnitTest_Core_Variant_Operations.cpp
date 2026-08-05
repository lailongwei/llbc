// The MIT License (MIT)

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

#include <deque>
#include <functional>
#include <iterator>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

// Coverage targets exercised by this test (collected by tools/coverage/run_unit_test_coverage.sh):
// @coverage-target: llbc/src/core/variant/Variant.cpp
// @coverage-target: llbc/include/llbc/core/variant/VariantInl.h
// @coverage-target: llbc/src/core/variant/VariantArithmetic.cpp
// @coverage-target: llbc/include/llbc/core/variant/VariantArithmeticInl.h
// @coverage-target: llbc/src/core/variant/VariantTraits.cpp

namespace
{

enum class VariantInlineTestEnum : uint16
{
    Value = 7,
};

} // namespace

// Variants are dynamically typed values used for configuration, reflection, and
// serialization. Verify type strings, explicit transitions, common state queries,
// and the stream representation used by diagnostics.
TEST(VariantOperationsTest, TypeStringsStateTransitionsAndFormatting)
{
    EXPECT_EQ(LLBC_VariantType::Type2Str(LLBC_VariantType::RAW_PTR), "pointer");
    EXPECT_EQ(LLBC_VariantType::Type2Str(0x7fff), "Nil");

    LLBC_Variant value;
    EXPECT_TRUE(value.Is<void>());
    EXPECT_TRUE(value.IsEmpty());
    EXPECT_EQ(value.Size(), 0lu);
    EXPECT_EQ(value.Capacity(), 0lu);

    value.Become(LLBC_VariantType::STR_DFT);
    value = LLBC_String("content");
    EXPECT_TRUE(value.Is<LLBC_Variant::Str>());
    EXPECT_FALSE(value.IsEmpty());
    EXPECT_EQ(value.Size(), 7lu);
    EXPECT_GE(value.Capacity(), value.Size());
    value.Clear();
    EXPECT_TRUE(value.IsEmpty());

    value.Become<LLBC_Variant::Seq>();
    value.SeqPushBack(1);
    EXPECT_TRUE(value.Is<LLBC_Variant::Seq>());
    EXPECT_EQ(value.Size(), 1lu);
    EXPECT_GE(value.Capacity(), value.Size());
    value.Clear();
    EXPECT_TRUE(value.IsEmpty());

    value.Become<LLBC_Variant::Dict>();
    value["name"] = LLBC_String("llbc");
    EXPECT_TRUE(value.Is<LLBC_Variant::Dict>());
    EXPECT_EQ(value.Size(), 1lu);
    EXPECT_EQ(value.Capacity(), 1lu);
    value.Clear();
    EXPECT_TRUE(value.IsEmpty());

    value = 42;
    value.Clear();
    EXPECT_TRUE(value.IsRaw());
    EXPECT_EQ(value.As<int>(), 0);

    const auto originalType = value.GetType();
    value.Become(static_cast<LLBC_VariantType::ENUM>(0x7fff));
    EXPECT_EQ(value.GetType(), originalType);
    value.Become(LLBC_VariantType::RAW_SINT32);
    EXPECT_EQ(value.As<int>(), 0);

    LLBC_Variant shown(LLBC_String("hello"));
    EXPECT_EQ(shown.ToString(), "Variant(string|hello)");
    std::ostringstream output;
    output << shown;
    EXPECT_EQ(output.str(), "Variant(string|hello)");
}

// The type predicates are used by generic configuration adapters. Invoke them
// through function pointers so their runtime template paths remain covered as
// well as compile-time deduction paths.
TEST(VariantOperationsTest, TypePredicatesClassifyRuntimeCategories)
{
    const auto isNilVoid = &LLBC_VariantType::IsNil<void>;
    const auto isNilVariant = &LLBC_VariantType::IsNil<LLBC_Variant>;
    const auto isNilInt = &LLBC_VariantType::IsNil<sint32>;
    EXPECT_TRUE(isNilVoid());
    EXPECT_TRUE(isNilVariant());
    EXPECT_FALSE(isNilInt());

    const auto isRawInt = &LLBC_VariantType::IsRaw<sint32>;
    const auto isRawPointer = &LLBC_VariantType::IsRaw<void *>;
    const auto isRawString = &LLBC_VariantType::IsRaw<LLBC_String>;
    EXPECT_TRUE(isRawInt());
    EXPECT_TRUE(isRawPointer());
    EXPECT_FALSE(isRawString());

    const auto isSignedInt = &LLBC_VariantType::IsSigned<sint32>;
    const auto isUnsignedInt = &LLBC_VariantType::IsUnsigned<uint64>;
    EXPECT_TRUE(isSignedInt());
    EXPECT_TRUE(isUnsignedInt());

    const auto isString = &LLBC_VariantType::IsStr<std::string>;
    const auto isSequence = &LLBC_VariantType::IsSeq<std::vector<int>>;
    const auto isDictionary = &LLBC_VariantType::IsDict<std::map<LLBC_String, int>>;
    EXPECT_TRUE(isString());
    EXPECT_TRUE(isSequence());
    EXPECT_TRUE(isDictionary());
}

// The integer-to-string cache is initialized by framework startup, but its public
// lifecycle methods must remain idempotent for embedders that explicitly manage
// startup and shutdown.
TEST(VariantOperationsTest, NumberToStringFastTableLifecycleIsIdempotent)
{
    LLBC_Variant::DestroyNumber2StrFastAccessTable();
    LLBC_Variant::DestroyNumber2StrFastAccessTable();

    LLBC_Variant fallback(42);
    EXPECT_EQ(fallback.As<LLBC_String>(), "42");

    LLBC_Variant::InitNumber2StrFastAccessTable();
    LLBC_Variant::InitNumber2StrFastAccessTable();
    EXPECT_EQ(fallback.As<LLBC_String>(), "42");
}

// Construction/assignment must preserve value semantics across raw, string,
// sequence, and dictionary types, while As<T>() provides the conversions used by
// configuration and reflection consumers.
TEST(VariantOperationsTest, ConstructionAssignmentAndConversions)
{
    LLBC_Variant raw(123);
    EXPECT_TRUE(raw.Is<int>());
    EXPECT_EQ(raw.As<int>(), 123);
    EXPECT_EQ(raw.As<LLBC_String>(), "123");

    LLBC_Variant numericText(LLBC_String("0x20"));
    EXPECT_TRUE(numericText.Is<LLBC_Variant::Str>());
    EXPECT_EQ(numericText.As<int>(), 32);
    EXPECT_TRUE(numericText.AsLooseBool());

    LLBC_Variant boolText(LLBC_String("yes"));
    EXPECT_TRUE(boolText.AsLooseBool());
    size_t stringLength = 0;
    const char *stringPtr = boolText.As<const char *>(&stringLength);
    EXPECT_EQ(std::string(stringPtr, stringLength), "yes");

    LLBC_Variant pointer(reinterpret_cast<void *>(static_cast<uintptr_t>(0x1234)));
    EXPECT_EQ(pointer.As<uintptr_t>(), 0x1234u);
    EXPECT_EQ(pointer.As<LLBC_String>(), "0x1234");

    LLBC_Variant::Seq sequence;
    sequence.emplace_back(1);
    sequence.emplace_back(LLBC_String("two"));
    LLBC_Variant sequenceValue(sequence);
    const auto asVector = sequenceValue.As<std::vector<LLBC_String>>();
    ASSERT_EQ(asVector.size(), 2lu);
    EXPECT_EQ(asVector[0], "1");
    EXPECT_EQ(asVector[1], "two");

    LLBC_Variant::Dict dictionary;
    dictionary.emplace(LLBC_Variant(LLBC_String("count")), LLBC_Variant(7));
    LLBC_Variant dictionaryValue(dictionary);
    const auto asMap = dictionaryValue.As<std::map<LLBC_String, int>>();
    ASSERT_EQ(asMap.size(), 1lu);
    EXPECT_EQ(asMap.at("count"), 7);

    LLBC_Variant copied = dictionaryValue;
    dictionaryValue["count"] = 8;
    EXPECT_EQ(copied["count"].As<int>(), 7);
    EXPECT_EQ(dictionaryValue["count"].As<int>(), 8);

    LLBC_Variant moved = std::move(copied);
    EXPECT_TRUE(moved.Is<LLBC_Variant::Dict>());
    EXPECT_EQ(moved["count"].As<int>(), 7);
    moved = LLBC_String("reassigned");
    EXPECT_EQ(moved.As<LLBC_String>(), "reassigned");

    EXPECT_TRUE(LLBC_Variant().As<LLBC_Variant::Seq>().empty());
    EXPECT_TRUE(LLBC_Variant().As<LLBC_Variant::Dict>().empty());
}

// Copy/move assignment has separate storage paths for strings, sequences,
// dictionaries, raw values, self-assignment, and type replacement. Exercise
// each path so a Variant remains a value type across configuration updates.
TEST(VariantOperationsTest, PreservesCopyMoveAndSelfAssignmentAcrossValueKinds)
{
    LLBC_Variant stringSource(LLBC_String("new-string"));
    LLBC_Variant stringTarget(LLBC_String("old-string"));
    stringTarget = stringSource;
    EXPECT_EQ(stringTarget.As<LLBC_String>(), "new-string");

    LLBC_Variant sequenceSource;
    sequenceSource.SeqBatchPushBack(1, 2);
    LLBC_Variant sequenceTarget;
    sequenceTarget.SeqPushBack(9);
    sequenceTarget = sequenceSource;
    EXPECT_EQ(sequenceTarget.As<LLBC_String>(), "[1,2]");
    LLBC_Variant copiedSequence(sequenceSource);
    EXPECT_EQ(copiedSequence.As<LLBC_String>(), "[1,2]");

    LLBC_Variant dictionarySource;
    dictionarySource["key"] = 3;
    LLBC_Variant dictionaryTarget;
    dictionaryTarget["old"] = 4;
    dictionaryTarget = dictionarySource;
    EXPECT_EQ(dictionaryTarget["key"].As<int>(), 3);

    LLBC_Variant rawTarget(1);
    rawTarget = LLBC_Variant(2);
    EXPECT_EQ(rawTarget.As<int>(), 2);

    LLBC_Variant crossType(LLBC_String("text"));
    crossType = sequenceSource;
    EXPECT_TRUE(crossType.Is<LLBC_Variant::Seq>());
    crossType = dictionarySource;
    EXPECT_TRUE(crossType.Is<LLBC_Variant::Dict>());
    crossType = LLBC_Variant(7);
    EXPECT_EQ(crossType.As<int>(), 7);
    LLBC_Variant *crossTypeAlias = &crossType;
    crossType = *crossTypeAlias;
    crossType = std::move(*crossTypeAlias);
    EXPECT_EQ(crossType.As<int>(), 7);

    LLBC_Variant sequenceForMove;
    sequenceForMove.SeqBatchPushBack(5, 6);
    LLBC_Variant movedSequence(std::move(sequenceForMove));
    EXPECT_EQ(movedSequence.As<LLBC_String>(), "[5,6]");

    LLBC_Variant movedStringTarget(LLBC_String("old"));
    LLBC_Variant movedStringSource(LLBC_String("new"));
    movedStringTarget = std::move(movedStringSource);
    EXPECT_EQ(movedStringTarget.As<LLBC_String>(), "new");

    LLBC_Variant movedSequenceTarget;
    movedSequenceTarget.SeqPushBack(0);
    LLBC_Variant movedSequenceSource;
    movedSequenceSource.SeqBatchPushBack(8, 9);
    movedSequenceTarget = std::move(movedSequenceSource);
    EXPECT_EQ(movedSequenceTarget.As<LLBC_String>(), "[8,9]");

    LLBC_Variant movedDictionaryTarget;
    movedDictionaryTarget["old"] = 0;
    LLBC_Variant movedDictionarySource;
    movedDictionarySource["new"] = 1;
    movedDictionaryTarget = std::move(movedDictionarySource);
    EXPECT_EQ(movedDictionaryTarget["new"].As<int>(), 1);

    LLBC_Variant differentSequenceDestination(0);
    LLBC_Variant differentSequenceSource;
    differentSequenceSource.SeqPushBack(10);
    differentSequenceDestination = std::move(differentSequenceSource);
    EXPECT_EQ(differentSequenceDestination.As<LLBC_String>(), "[10]");

    LLBC_Variant differentDictionaryDestination(LLBC_String("text"));
    LLBC_Variant differentDictionarySource;
    differentDictionarySource["value"] = 11;
    differentDictionaryDestination = std::move(differentDictionarySource);
    EXPECT_EQ(differentDictionaryDestination["value"].As<int>(), 11);

    LLBC_Variant differentStringDestination(0);
    LLBC_Variant differentStringSource(LLBC_String("moved-string"));
    differentStringDestination = std::move(differentStringSource);
    EXPECT_EQ(differentStringDestination.As<LLBC_String>(), "moved-string");

    LLBC_Variant differentRawDestination(LLBC_String("text"));
    LLBC_Variant differentRawSource(12);
    differentRawDestination = std::move(differentRawSource);
    EXPECT_EQ(differentRawDestination.As<int>(), 12);
}

// Dynamic values are frequently converted at configuration boundaries. Check
// empty/nil fallbacks, number parsing, textual rendering, pair extraction, and
// non-empty container hashing through the public conversion API.
TEST(VariantOperationsTest, ConvertsBoundaryValuesAndRendersMixedContainers)
{
    LLBC_Variant nil;
    EXPECT_FALSE(nil.As<bool>());
    EXPECT_DOUBLE_EQ(nil.As<double>(), 0.0);
    EXPECT_EQ(nil.As<sint64>(), 0);
    EXPECT_TRUE(nil.As<std::string>().empty());
    EXPECT_TRUE(nil.As<std::string_view>().empty());
    const auto nilPair = nil.As<std::pair<int, int>>();
    EXPECT_EQ(nilPair, std::make_pair(0, 0));
    const LLBC_Variant &constNil = nil;
    EXPECT_TRUE(constNil["missing"].Is<void>());

    LLBC_Variant sequence;
    sequence.SeqPushBack(5);
    EXPECT_TRUE(sequence.As<bool>());
    EXPECT_EQ(sequence.As<uint64>(), 0u);
    const auto singleElementPair = sequence.As<std::pair<int, int>>();
    EXPECT_EQ(singleElementPair, std::make_pair(5, 0));

    LLBC_Variant emptyText(LLBC_String(""));
    EXPECT_DOUBLE_EQ(emptyText.As<double>(), 0.0);
    EXPECT_EQ(emptyText.As<uint64>(), 0u);
    EXPECT_TRUE(LLBC_Variant(LLBC_String("nonempty")).As<bool>());
    LLBC_Variant truthyDictionary;
    truthyDictionary["key"] = 1;
    EXPECT_TRUE(truthyDictionary.As<bool>());
    EXPECT_EQ(truthyDictionary.As<sint64>(), 0);
    LLBC_Variant decimalText(LLBC_String(" 1.25 "));
    EXPECT_DOUBLE_EQ(decimalText.As<double>(), 1.25);
    LLBC_Variant negativeText(LLBC_String("-3"));
    EXPECT_DOUBLE_EQ(negativeText.As<double>(), -3.0);
    LLBC_Variant positiveText(LLBC_String("4"));
    EXPECT_DOUBLE_EQ(positiveText.As<double>(), 4.0);
    EXPECT_EQ(LLBC_Variant(LLBC_String("0X20")).As<int>(), 32);
    EXPECT_EQ(LLBC_Variant(0.0).As<int>(), 0);
    EXPECT_EQ(LLBC_Variant(1.5).As<int>(), 1);

    const std::string viewSource = "view-source";
    LLBC_Variant fromStringView {std::string_view(viewSource)};
    EXPECT_EQ(fromStringView.As<LLBC_String>(), "view-source");
    LLBC_Variant assignedStringView(LLBC_String("old"));
    assignedStringView = std::string_view("new-view");
    EXPECT_EQ(assignedStringView.As<LLBC_String>(), "new-view");
    LLBC_Variant assignedCString(LLBC_String("old"));
    assignedCString = "new-c-string";
    EXPECT_EQ(assignedCString.As<LLBC_String>(), "new-c-string");
    EXPECT_EQ(LLBC_Variant(LLBC_String("view")).As<std::string_view>(), "view");

    EXPECT_EQ(LLBC_Variant(true).As<std::string>(), "true");
    EXPECT_EQ(LLBC_Variant(false).As<std::string>(), "false");
    EXPECT_EQ(LLBC_Variant(1.5).As<std::string>(), "1.500000");
    EXPECT_EQ(LLBC_Variant(static_cast<uint32>(7)).As<LLBC_String>(), "7");
    EXPECT_EQ(LLBC_Variant(static_cast<uint64>(1000000000000ULL)).As<std::string>(),
              "1000000000000");

    LLBC_Variant renderedSequence;
    renderedSequence.SeqBatchPushBack(LLBC_String("left"), 2);
    EXPECT_EQ(renderedSequence.As<std::string>(), "[left,2]");

    LLBC_Variant renderedDictionary;
    renderedDictionary[1] = LLBC_String("one");
    EXPECT_EQ(renderedDictionary.As<std::string>(), "{1:one}");
    EXPECT_EQ(renderedDictionary.Count(LLBC_Variant(1)), 1lu);

    LLBC_Variant repeatedText(LLBC_String("abab"));
    EXPECT_EQ(repeatedText.Count(LLBC_Variant(LLBC_String("ab"))), 2lu);
    EXPECT_EQ(repeatedText.Count(LLBC_String("ab")), 2lu);
    EXPECT_EQ(repeatedText.Count(std::string("ab")), 2lu);
    EXPECT_EQ(repeatedText.Count(std::string_view("ab")), 2lu);
    EXPECT_EQ(repeatedText.Count(7), 0lu);
    EXPECT_EQ(LLBC_Variant(1).Count(1), 0lu);
    EXPECT_EQ(LLBC_Variant(LLBC_String("")).Count("x"), 0lu);

    const std::hash<LLBC_Variant> hash;
    EXPECT_NE(hash(LLBC_Variant(1.5)), 0lu);
    EXPECT_NE(hash(renderedSequence), 10000lu);
    EXPECT_NE(hash(renderedDictionary), 20000lu);
}

// Container mutators deliberately normalize a scalar Variant into the matching
// empty container before examining iterator arguments. This lets callers reset
// a value into a sequence/dictionary safely during schema migration.
TEST(VariantOperationsTest, NormalizesContainerMutatorsBeforeUsingIterators)
{
    LLBC_Variant sequenceSource;
    sequenceSource.SeqPushBack(1);
    const auto sequenceIter = sequenceSource.SeqBegin();
    const LLBC_Variant &constSequenceSource = sequenceSource;
    const auto sequenceConstIter = constSequenceSource.SeqBegin();
    const auto sequenceConstEnd = constSequenceSource.SeqEnd();

    LLBC_Variant sequenceInsertOne(1);
    EXPECT_EQ(sequenceInsertOne.SeqInsert(sequenceIter, 2), sequenceInsertOne.SeqEnd());
    EXPECT_TRUE(sequenceInsertOne.Is<LLBC_Variant::Seq>());
    EXPECT_TRUE(sequenceInsertOne.IsEmpty());

    LLBC_Variant sequenceInsertMany(1);
    EXPECT_EQ(sequenceInsertMany.SeqInsert(sequenceIter, 2, 3), sequenceInsertMany.SeqEnd());
    EXPECT_TRUE(sequenceInsertMany.IsEmpty());

    LLBC_Variant sequenceInsertRange(1);
    EXPECT_EQ(sequenceInsertRange.SeqInsert(sequenceIter, sequenceConstIter, sequenceConstEnd),
              sequenceInsertRange.SeqEnd());
    EXPECT_TRUE(sequenceInsertRange.IsEmpty());

    LLBC_Variant sequenceBatchInsert(1);
    EXPECT_EQ(sequenceBatchInsert.SeqBatchInsert(sequenceIter, 4, 5),
              sequenceBatchInsert.SeqEnd());
    EXPECT_TRUE(sequenceBatchInsert.IsEmpty());

    LLBC_Variant sequenceEraseIter(1);
    EXPECT_EQ(sequenceEraseIter.SeqErase(sequenceIter), sequenceEraseIter.SeqEnd());
    LLBC_Variant sequenceEraseConstIter(1);
    EXPECT_EQ(sequenceEraseConstIter.SeqErase(sequenceConstIter), sequenceEraseConstIter.SeqEnd());
    LLBC_Variant sequenceEraseRange(1);
    EXPECT_EQ(sequenceEraseRange.SeqErase(sequenceIter, sequenceSource.SeqEnd()),
              sequenceEraseRange.SeqEnd());
    LLBC_Variant sequenceEraseConstRange(1);
    EXPECT_EQ(sequenceEraseConstRange.SeqErase(sequenceConstIter, sequenceConstEnd),
              sequenceEraseConstRange.SeqEnd());

    LLBC_Variant sequenceEraseValue(1);
    EXPECT_EQ(sequenceEraseValue.SeqErase(1), 0lu);
    LLBC_Variant emptySequence;
    emptySequence.Become<LLBC_Variant::Seq>();
    EXPECT_EQ(emptySequence.SeqErase(1), 0lu);

    LLBC_Variant dictionarySource;
    dictionarySource["key"] = 1;
    const auto dictionaryIter = dictionarySource.DictBegin();
    const LLBC_Variant &constDictionarySource = dictionarySource;
    const auto dictionaryConstIter = constDictionarySource.DictBegin();
    const auto dictionaryConstEnd = constDictionarySource.DictEnd();

    LLBC_Variant dictionaryEraseIter(1);
    EXPECT_EQ(dictionaryEraseIter.DictErase(dictionaryIter), dictionaryEraseIter.DictEnd());
    LLBC_Variant dictionaryEraseConstIter(1);
    EXPECT_EQ(dictionaryEraseConstIter.DictErase(dictionaryConstIter),
              dictionaryEraseConstIter.DictEnd());
    LLBC_Variant dictionaryEraseRange(1);
    EXPECT_EQ(dictionaryEraseRange.DictErase(dictionaryIter, dictionarySource.DictEnd()),
              dictionaryEraseRange.DictEnd());
    LLBC_Variant dictionaryEraseConstRange(1);
    EXPECT_EQ(dictionaryEraseConstRange.DictErase(dictionaryConstIter, dictionaryConstEnd),
              dictionaryEraseConstRange.DictEnd());
    LLBC_Variant dictionaryEraseKey(1);
    EXPECT_EQ(dictionaryEraseKey.DictErase("key"), 0lu);
    EXPECT_TRUE(dictionaryEraseKey.Is<LLBC_Variant::Dict>());
}

// Sequence and dictionary helpers adapt a Variant into a heterogeneous container
// while retaining convenient indexed/keyed access for configuration trees.
TEST(VariantOperationsTest, SequenceAndDictionaryHelpers)
{
    LLBC_Variant sequence;
    sequence.SeqPushBack(1);
    sequence.SeqBatchPushBack(2, 3);
    ASSERT_EQ(sequence.Size(), 3lu);
    sequence.SeqInsert(sequence.SeqBegin() + 1, 9);
    EXPECT_EQ(sequence[0].As<int>(), 1);
    EXPECT_EQ(sequence[1].As<int>(), 9);
    EXPECT_EQ(sequence[2].As<int>(), 2);

    sequence.SeqResize(5, 7);
    EXPECT_EQ(sequence.Size(), 5lu);
    EXPECT_EQ(sequence[4].As<int>(), 7);
    EXPECT_EQ(sequence.SeqErase(7, 1), 1lu);
    EXPECT_EQ(sequence.Size(), 4lu);
    EXPECT_EQ(sequence.SeqEraseIf([](const LLBC_Variant &item) { return item == 9; }), 1lu);
    EXPECT_FALSE(sequence.Contains(9));
    EXPECT_EQ(sequence.Count(2), 1lu);
    sequence.SeqPopBack();
    EXPECT_EQ(sequence.Size(), 2lu);

    LLBC_Variant dictionary;
    const auto inserted = dictionary.DictInsert("alpha", 1);
    EXPECT_TRUE(inserted.second);
    EXPECT_FALSE(dictionary.DictInsert("alpha", 2).second);
    dictionary["beta"] = 2;
    EXPECT_TRUE(dictionary.Contains("alpha"));
    EXPECT_EQ(dictionary.Count("beta"), 1lu);
    EXPECT_EQ(dictionary.DictFind("beta")->second.As<int>(), 2);
    EXPECT_EQ(dictionary.DictErase("alpha", "missing"), 1lu);
    EXPECT_FALSE(dictionary.Contains("alpha"));
    EXPECT_TRUE(static_cast<const LLBC_Variant &>(dictionary)["missing"].Is<void>());

    LLBC_Variant indexedSequence;
    indexedSequence.Become<LLBC_Variant::Seq>();
    indexedSequence[3] = 33;
    EXPECT_EQ(indexedSequence.Size(), 4lu);
    EXPECT_EQ(indexedSequence[3].As<int>(), 33);
    EXPECT_TRUE(static_cast<const LLBC_Variant &>(indexedSequence)[99].Is<void>());
}

// Inline helpers provide container-like mutation and conversion on top of the
// dynamically typed value. Exercise iterator, range, reverse, conversion, and
// move paths so configuration code can use Variant as a real sequence wrapper.
TEST(VariantOperationsTest, ExercisesInlineStringAndSequenceMutators)
{
    using StringIntMap = std::map<LLBC_String, int>;

    EXPECT_TRUE(LLBC_VariantType::IsRaw<sint32>());
    EXPECT_TRUE(LLBC_VariantType::IsStr<LLBC_String>());
    EXPECT_TRUE(LLBC_VariantType::IsSeq<std::vector<int>>());
    EXPECT_TRUE(LLBC_VariantType::IsDict<StringIntMap>());
    EXPECT_TRUE(LLBC_VariantType::IsSupportedIntegralType<uint64>());
    EXPECT_FALSE(LLBC_VariantType::IsSupportedIntegralType<wchar_t>());
    EXPECT_TRUE(LLBC_VariantType::IsSupportedEnumType<VariantInlineTestEnum>());
    EXPECT_TRUE(LLBC_VariantType::IsSupportedFloatingPointType<double>());
    EXPECT_FALSE(LLBC_VariantType::IsSupportedFloatingPointType<ldouble>());

    LLBC_Variant text;
    text.StrReserve(16);
    text.StrResize(3, 'x');
    EXPECT_EQ(text.As<LLBC_String>(), "xxx");
    EXPECT_EQ(text.Count("x"), 3lu);
    EXPECT_TRUE(text.Contains(std::string_view("xx")));
    text.StrResize(1);
    EXPECT_EQ(text.As<LLBC_String>(), "x");

    LLBC_Variant raw(123);
    const std::string_view rawView = raw.As<std::string_view>();
    EXPECT_EQ(rawView, "123");
    size_t rawLength = 0;
    EXPECT_EQ(std::string(raw.As<const char *>(&rawLength), rawLength), "123");

    LLBC_Variant sequence;
    sequence.SeqReserve(16);
    EXPECT_EQ(sequence.SeqBegin(), sequence.SeqEnd());
    sequence.SeqBatchPushBack(2, 3);
    EXPECT_EQ(sequence.SeqInsert(sequence.SeqBegin(), 1)->As<int>(), 1);
    sequence.SeqInsert(sequence.SeqEnd(), 2, 4);

    LLBC_Variant::Seq source;
    source.emplace_back(7);
    source.emplace_back(8);
    const LLBC_Variant::SeqConstIter sourceBegin = source.begin();
    const LLBC_Variant::SeqConstIter sourceEnd = source.end();
    sequence.SeqInsert(sequence.SeqEnd(), sourceBegin, sourceEnd);
    sequence.SeqBatchInsert(sequence.SeqBegin() + 1, 9, 10);

    ASSERT_EQ(sequence.Size(), 9lu);
    EXPECT_EQ(sequence.SeqFront().As<int>(), 1);
    EXPECT_EQ(sequence.SeqBack().As<int>(), 8);
    EXPECT_EQ(sequence.SeqReverseBegin()->As<int>(), 8);
    EXPECT_EQ((sequence.SeqReverseEnd() - sequence.SeqReverseBegin()), 9);
    EXPECT_EQ(sequence.Count(4), 2lu);
    EXPECT_TRUE(sequence.Contains(10));
    EXPECT_EQ(sequence.As<LLBC_String>(), "[1,9,10,2,3,4,4,7,8]");

    const auto asDeque = sequence.As<std::deque<int>>();
    const auto asList = sequence.As<std::list<int>>();
    const auto asQueue = sequence.As<std::queue<int>>();
    const auto asSet = sequence.As<std::set<int>>();
    const auto asUnorderedSet = sequence.As<std::unordered_set<int>>();
    EXPECT_EQ(asDeque.front(), 1);
    EXPECT_EQ(asList.back(), 8);
    EXPECT_EQ(asQueue.front(), 1);
    EXPECT_EQ(asSet.count(4), 1lu);
    EXPECT_EQ(asUnorderedSet.count(10), 1lu);

    LLBC_Variant pairSequence;
    pairSequence.SeqBatchPushBack(LLBC_String("left"), 42);
    const auto pair = pairSequence.As<std::pair<std::string, int>>();
    EXPECT_EQ(pair.first, "left");
    EXPECT_EQ(pair.second, 42);

    sequence.SeqErase(sequence.SeqBegin());
    const LLBC_Variant &constSequence = sequence;
    sequence.SeqErase(constSequence.SeqBegin());
    sequence.SeqErase(sequence.SeqBegin(), sequence.SeqBegin() + 2);
    const LLBC_Variant &constSequenceAfterRangeErase = sequence;
    sequence.SeqErase(constSequenceAfterRangeErase.SeqBegin(),
                      constSequenceAfterRangeErase.SeqBegin() + 1);
    EXPECT_EQ(sequence.Size(), 4lu);

    LLBC_Variant eraseSequence;
    eraseSequence.SeqBatchPushBack(1, 2, 1, 3);
    EXPECT_EQ(eraseSequence.SeqErase(1, 1, false, false), 1lu);
    EXPECT_EQ(eraseSequence.Count(1), 1lu);
    EXPECT_EQ(eraseSequence.SeqEraseIf(
                  [](const LLBC_Variant &item) { return item.As<int>() % 2 == 0; }),
              1lu);
    EXPECT_FALSE(eraseSequence.Contains(2));

    LLBC_Variant::Seq movableSequence;
    movableSequence.emplace_back(11);
    movableSequence.emplace_back(12);
    LLBC_Variant movedSequence(std::move(movableSequence));
    LLBC_Variant moveAssignedSequence;
    moveAssignedSequence = std::move(movedSequence);
    ASSERT_EQ(moveAssignedSequence.Size(), 2lu);
    EXPECT_EQ(moveAssignedSequence[0].As<int>(), 11);
    EXPECT_EQ(moveAssignedSequence[1].As<int>(), 12);
}

// Dictionary iterators and hashes make Variants usable as dynamic maps and map
// keys. Check all insertion/erasure forms plus stable string representations.
TEST(VariantOperationsTest, ExercisesInlineDictionaryMutatorsAndHashing)
{
    LLBC_Variant dictionary;
    EXPECT_EQ(dictionary.DictBegin(), dictionary.DictEnd());
    const LLBC_Variant::Dict::value_type alpha(LLBC_Variant(LLBC_String("alpha")), LLBC_Variant(1));
    EXPECT_TRUE(dictionary.DictInsert(alpha).second);
    EXPECT_TRUE(dictionary.DictInsert(
                    LLBC_Variant::Dict::value_type(LLBC_Variant(LLBC_String("beta")),
                                                   LLBC_Variant(2)))
                    .second);
    EXPECT_TRUE(dictionary.DictInsert("gamma", 3).second);
    EXPECT_EQ(dictionary.DictReverseBegin()->first.As<LLBC_String>(), "gamma");
    EXPECT_EQ(std::distance(dictionary.DictReverseBegin(), dictionary.DictReverseEnd()), 3);

    dictionary.DictFind("beta")->second = 20;
    const LLBC_Variant &constDictionary = dictionary;
    EXPECT_EQ(constDictionary.DictFind("beta")->second.As<int>(), 20);
    EXPECT_EQ(dictionary.Count("beta"), 1lu);
    EXPECT_TRUE(dictionary.Contains("gamma"));
    EXPECT_EQ(dictionary.As<LLBC_String>(), "{alpha:1,beta:20,gamma:3}");

    LLBC_Variant eraseByIterator = dictionary;
    eraseByIterator.DictErase(eraseByIterator.DictFind("alpha"));
    EXPECT_FALSE(eraseByIterator.Contains("alpha"));

    LLBC_Variant eraseByConstIterator = dictionary;
    const LLBC_Variant &constEraseByConstIterator = eraseByConstIterator;
    eraseByConstIterator.DictErase(constEraseByConstIterator.DictFind("beta"));
    EXPECT_FALSE(eraseByConstIterator.Contains("beta"));

    LLBC_Variant eraseByRange = dictionary;
    eraseByRange.DictErase(eraseByRange.DictBegin(), eraseByRange.DictEnd());
    EXPECT_TRUE(eraseByRange.IsEmpty());

    LLBC_Variant eraseByConstRange = dictionary;
    const LLBC_Variant &constEraseByRange = eraseByConstRange;
    eraseByConstRange.DictErase(constEraseByRange.DictBegin(), constEraseByRange.DictEnd());
    EXPECT_TRUE(eraseByConstRange.IsEmpty());

    EXPECT_EQ(dictionary.DictErase("alpha", "gamma"), 2lu);
    EXPECT_EQ(dictionary.Size(), 1lu);
    EXPECT_TRUE(dictionary.Contains("beta"));

    LLBC_Variant emptySequence;
    emptySequence.Become<LLBC_Variant::Seq>();
    LLBC_Variant emptyDictionary;
    emptyDictionary.Become<LLBC_Variant::Dict>();
    EXPECT_EQ(emptySequence.As<LLBC_String>(), "[]");
    EXPECT_EQ(emptyDictionary.As<LLBC_String>(), "{}");

    const std::hash<LLBC_Variant> hash;
    EXPECT_EQ(hash(LLBC_Variant()), 0lu);
    EXPECT_EQ(hash(emptySequence), 10000lu);
    EXPECT_EQ(hash(emptyDictionary), 20000lu);
    EXPECT_EQ(hash(dictionary), hash(dictionary));
    EXPECT_EQ(hash(LLBC_Variant(42)), 42lu);
    EXPECT_EQ(hash(LLBC_Variant(LLBC_String("value"))),
              hash(LLBC_Variant(LLBC_String("value"))));
}

// Variant serialization underpins nested configuration and packet payloads.
// Verify every supported category, empty containers, and malformed streams that
// must clear the destination instead of leaving partially decoded data behind.
TEST(VariantOperationsTest, SerializesDeserializesAndRejectsIncompleteStreams)
{
    const auto roundTrip = [](const LLBC_Variant &source) {
        LLBC_Stream stream;
        source.Serialize(stream);
        LLBC_Variant destination;
        EXPECT_TRUE(destination.Deserialize(stream));
        return destination;
    };

    LLBC_Variant raw(123);
    EXPECT_EQ(roundTrip(raw).As<int>(), 123);

    LLBC_Variant string(LLBC_String("text"));
    EXPECT_EQ(roundTrip(string).As<LLBC_String>(), "text");

    LLBC_Variant sequence;
    sequence.SeqBatchPushBack(1, LLBC_String("two"));
    const auto sequenceResult = roundTrip(sequence);
    ASSERT_TRUE(sequenceResult.Is<LLBC_Variant::Seq>());
    ASSERT_EQ(sequenceResult.Size(), 2lu);
    EXPECT_EQ(sequenceResult[0].As<int>(), 1);
    EXPECT_EQ(sequenceResult[1].As<LLBC_String>(), "two");

    LLBC_Variant dictionary;
    dictionary["number"] = 7;
    dictionary["text"] = LLBC_String("value");
    const auto dictionaryResult = roundTrip(dictionary);
    ASSERT_TRUE(dictionaryResult.Is<LLBC_Variant::Dict>());
    EXPECT_EQ(dictionaryResult["number"].As<int>(), 7);
    EXPECT_EQ(dictionaryResult["text"].As<LLBC_String>(), "value");

    LLBC_Variant emptySequence;
    emptySequence.Become<LLBC_Variant::Seq>();
    EXPECT_TRUE(roundTrip(emptySequence).As<LLBC_Variant::Seq>().empty());
    LLBC_Variant emptyDictionary;
    emptyDictionary.Become<LLBC_Variant::Dict>();
    EXPECT_TRUE(roundTrip(emptyDictionary).As<LLBC_Variant::Dict>().empty());
    EXPECT_TRUE(roundTrip(LLBC_Variant()).Is<void>());

    LLBC_Variant destination(LLBC_String("unchanged"));
    LLBC_Stream noType;
    EXPECT_FALSE(destination.Deserialize(noType));
    EXPECT_EQ(destination.As<LLBC_String>(), "unchanged");

    LLBC_Stream incompleteRaw;
    incompleteRaw.Write(LLBC_VariantType::RAW_SINT32);
    EXPECT_FALSE(destination.Deserialize(incompleteRaw));
    EXPECT_TRUE(destination.Is<void>());

    LLBC_Stream incompleteString;
    incompleteString.Write(LLBC_VariantType::STR_DFT);
    EXPECT_FALSE(destination.Deserialize(incompleteString));
    EXPECT_TRUE(destination.Is<void>());

    LLBC_Stream incompleteSequence;
    incompleteSequence.Write(LLBC_VariantType::SEQ_DFT);
    EXPECT_FALSE(destination.Deserialize(incompleteSequence));
    EXPECT_TRUE(destination.Is<void>());

    LLBC_Stream incompleteSequenceItem;
    incompleteSequenceItem.Write(LLBC_VariantType::SEQ_DFT);
    incompleteSequenceItem.Write(static_cast<uint32>(1));
    EXPECT_FALSE(destination.Deserialize(incompleteSequenceItem));
    EXPECT_TRUE(destination.Is<void>());

    LLBC_Stream incompleteDictionary;
    incompleteDictionary.Write(LLBC_VariantType::DICT_DFT);
    EXPECT_FALSE(destination.Deserialize(incompleteDictionary));
    EXPECT_TRUE(destination.Is<void>());

    LLBC_Stream incompleteDictionaryItem;
    incompleteDictionaryItem.Write(LLBC_VariantType::DICT_DFT);
    incompleteDictionaryItem.Write(static_cast<uint32>(1));
    EXPECT_FALSE(destination.Deserialize(incompleteDictionaryItem));
    EXPECT_TRUE(destination.Is<void>());
}

// Raw Variant arithmetic dispatches by both operand types. Cover every supported
// scalar left-hand type and the add/subtract/multiply/divide/modulo behaviors.
TEST(VariantOperationsTest, ArithmeticDispatchesAcrossRawScalarTypes)
{
    LLBC_Variant boolValue(true);
    boolValue += true;
    EXPECT_EQ(boolValue.As<int>(), 2);

    LLBC_Variant sint8Value(static_cast<sint8>(4));
    sint8Value += static_cast<sint8>(3);
    EXPECT_EQ(sint8Value.As<sint8>(), 7);

    LLBC_Variant uint8Value(static_cast<uint8>(4));
    uint8Value += static_cast<uint8>(3);
    EXPECT_EQ(uint8Value.As<uint8>(), 7u);

    LLBC_Variant sint16Value(static_cast<sint16>(4));
    sint16Value += static_cast<sint16>(3);
    EXPECT_EQ(sint16Value.As<sint16>(), 7);

    LLBC_Variant uint16Value(static_cast<uint16>(4));
    uint16Value += static_cast<uint16>(3);
    EXPECT_EQ(uint16Value.As<uint16>(), 7u);

    LLBC_Variant sint32Value(static_cast<sint32>(4));
    sint32Value += static_cast<sint32>(3);
    EXPECT_EQ(sint32Value.As<sint32>(), 7);

    LLBC_Variant uint32Value(static_cast<uint32>(4));
    uint32Value += static_cast<uint32>(3);
    EXPECT_EQ(uint32Value.As<uint32>(), 7u);

    LLBC_Variant longValue(4L);
    longValue += 3L;
    EXPECT_EQ(longValue.As<long>(), 7L);

    LLBC_Variant ulongValue(4UL);
    ulongValue += 3UL;
    EXPECT_EQ(ulongValue.As<ulong>(), 7UL);

    LLBC_Variant sint64Value(static_cast<sint64>(4));
    sint64Value += static_cast<sint64>(3);
    EXPECT_EQ(sint64Value.As<sint64>(), 7);

    LLBC_Variant uint64Value(static_cast<uint64>(4));
    uint64Value += static_cast<uint64>(3);
    EXPECT_EQ(uint64Value.As<uint64>(), 7u);

    LLBC_Variant floatValue(4.5f);
    floatValue += 0.5f;
    EXPECT_FLOAT_EQ(floatValue.As<float>(), 5.0f);

    LLBC_Variant doubleValue(4.5);
    doubleValue += 0.5;
    EXPECT_DOUBLE_EQ(doubleValue.As<double>(), 5.0);

    LLBC_Variant boolDivision(true);
    boolDivision /= false;
    EXPECT_EQ(boolDivision.As<int>(), 0);
    LLBC_Variant boolModulo(true);
    boolModulo %= false;
    EXPECT_EQ(boolModulo.As<int>(), 0);

    LLBC_Variant doubleDivision(4.5);
    doubleDivision /= 0.0;
    EXPECT_DOUBLE_EQ(doubleDivision.As<double>(), 0.0);
    LLBC_Variant doubleModulo(4.5);
    doubleModulo %= 0.0;
    EXPECT_DOUBLE_EQ(doubleModulo.As<double>(), 4.0);

    LLBC_Variant arithmetic(20);
    EXPECT_EQ((arithmetic - 3).As<int>(), 17);
    EXPECT_EQ((arithmetic * 3).As<int>(), 60);
    EXPECT_EQ((arithmetic / 4).As<int>(), 5);
    EXPECT_EQ((arithmetic % 6).As<int>(), 2);
    EXPECT_EQ((arithmetic / 0).As<int>(), 0);
    EXPECT_EQ((arithmetic % 0).As<int>(), 0);
}

// Addition is safe for every raw combination and drives the arithmetic dispatch
// matrix used by dynamic numeric coercion. The values are deliberately small so
// the test exercises type routing rather than overflow behavior.
TEST(VariantOperationsTest, AdditionCoversRawTypeDispatchMatrix)
{
    const void *pointerValue = reinterpret_cast<void *>(static_cast<uintptr_t>(1));
    const std::vector<LLBC_Variant> values = {
        LLBC_Variant(true),
        LLBC_Variant(static_cast<sint8>(1)),
        LLBC_Variant(static_cast<uint8>(1)),
        LLBC_Variant(static_cast<sint16>(1)),
        LLBC_Variant(static_cast<uint16>(1)),
        LLBC_Variant(static_cast<sint32>(1)),
        LLBC_Variant(static_cast<uint32>(1)),
        LLBC_Variant(1L),
        LLBC_Variant(1UL),
        LLBC_Variant(pointerValue),
        LLBC_Variant(static_cast<sint64>(1)),
        LLBC_Variant(static_cast<uint64>(1)),
        LLBC_Variant(1.0f),
        LLBC_Variant(1.0),
    };

    for (const auto &leftTemplate : values)
    {
        for (const auto &right : values)
        {
            LLBC_Variant result(leftTemplate);
            result += right;
            EXPECT_TRUE(result.IsRaw())
                << "left type: " << result.TypeToString()
                << ", right type: " << right.TypeToString();
        }
    }
}

// VariantTraits also defines collection and string algebra used by dynamic
// configuration values: merge, removal, intersection, repetition, and coercion.
TEST(VariantOperationsTest, AppliesStringSequenceDictionaryAndNilAlgebra)
{
    LLBC_Variant nil;
    nil += 7;
    EXPECT_EQ(nil.As<int>(), 7);
    nil *= LLBC_Variant();
    EXPECT_TRUE(nil.Is<void>());

    LLBC_Variant string(LLBC_String("banana"));
    string -= LLBC_String("an");
    EXPECT_EQ(string.As<LLBC_String>(), "ba");
    string = LLBC_String("ab");
    string *= 3;
    EXPECT_EQ(string.As<LLBC_String>(), "ababab");
    string += 42;
    EXPECT_EQ(string.As<LLBC_String>(), "ababab42");

    LLBC_Variant sequence;
    sequence.SeqBatchPushBack(1, 2);
    sequence += 3;
    ASSERT_EQ(sequence.Size(), 3lu);
    EXPECT_EQ(sequence[2].As<int>(), 3);
    sequence *= 2;
    ASSERT_EQ(sequence.Size(), 6lu);
    EXPECT_EQ(sequence[0].As<int>(), 1);
    EXPECT_EQ(sequence[3].As<int>(), 1);
    sequence -= 2;
    EXPECT_EQ(sequence.Count(2), 0lu);

    LLBC_Variant leftDict;
    leftDict["one"] = 1;
    leftDict["two"] = 2;
    LLBC_Variant rightDict;
    rightDict["two"] = 20;
    rightDict["three"] = 3;
    leftDict += rightDict;
    EXPECT_EQ(leftDict.Size(), 3lu);
    EXPECT_EQ(leftDict["two"].As<int>(), 2); // Existing value wins on map insert.
    leftDict -= LLBC_Variant(LLBC_String("one"));
    EXPECT_TRUE(static_cast<const LLBC_Variant &>(leftDict)["one"].Is<void>());
    leftDict *= rightDict;
    EXPECT_EQ(leftDict.Size(), 2lu);
    EXPECT_TRUE(leftDict.Contains("two"));
    EXPECT_TRUE(leftDict.Contains("three"));
}

// Collection additions are used when configuration fragments are merged at
// runtime: dictionaries contribute keys to sequences, sequences concatenate,
// strings concatenate only scalar-like values, and raw values promote to text
// when a string is introduced.
TEST(VariantOperationsTest, MergesMixedCollectionOperandsAndPromotesRawStrings)
{
    LLBC_Variant sequence;
    sequence.SeqPushBack(LLBC_String("base"));
    LLBC_Variant dictionary;
    dictionary["alpha"] = 1;
    dictionary["beta"] = 2;
    sequence += dictionary;
    ASSERT_EQ(sequence.Size(), 3lu);
    EXPECT_EQ(sequence[1].As<LLBC_String>(), "alpha");
    EXPECT_EQ(sequence[2].As<LLBC_String>(), "beta");

    LLBC_Variant appended;
    appended.SeqBatchPushBack(3, 4);
    sequence += appended;
    ASSERT_EQ(sequence.Size(), 5lu);
    EXPECT_EQ(sequence[3].As<int>(), 3);
    EXPECT_EQ(sequence[4].As<int>(), 4);

    LLBC_Variant string(LLBC_String("left"));
    string += LLBC_String("-right");
    EXPECT_EQ(string.As<LLBC_String>(), "left-right");
    string += sequence;
    EXPECT_EQ(string.As<LLBC_String>(), "left-right");

    LLBC_Variant raw(7);
    raw += LLBC_String("days");
    EXPECT_TRUE(raw.Is<LLBC_Variant::Str>());
    EXPECT_EQ(raw.As<LLBC_String>(), "7days");

    EXPECT_EQ(LLBC_VariantTraits::add(LLBC_Variant(20), LLBC_Variant(22)).As<int>(), 42);
}

// Subtraction removes dictionary keys and every matching sequence element.
// String/raw coercions make the same operation useful for dynamic text filters.
TEST(VariantOperationsTest, RemovesDictionarySequenceAndStringMembers)
{
    LLBC_Variant nil;
    nil -= LLBC_String("replacement");
    EXPECT_EQ(nil.As<LLBC_String>(), "replacement");

    LLBC_Variant dictionary;
    dictionary["one"] = 1;
    dictionary["two"] = 2;
    dictionary["three"] = 3;
    LLBC_Variant removeDictionary;
    removeDictionary["one"] = 0;
    removeDictionary["two"] = 0;
    dictionary -= removeDictionary;
    EXPECT_EQ(dictionary.Size(), 1lu);
    EXPECT_TRUE(dictionary.Contains("three"));

    LLBC_Variant emptyingDictionary;
    emptyingDictionary["one"] = 1;
    emptyingDictionary["two"] = 2;
    LLBC_Variant removeKeys;
    removeKeys.SeqBatchPushBack(LLBC_String("one"), LLBC_String("two"));
    emptyingDictionary -= removeKeys;
    EXPECT_TRUE(emptyingDictionary.IsEmpty());

    LLBC_Variant sequence;
    sequence.SeqBatchPushBack(1, 2, 2, 3);
    LLBC_Variant::Dict dictKeys;
    dictKeys.emplace(LLBC_Variant(2), LLBC_Variant());
    sequence -= LLBC_Variant(dictKeys);
    ASSERT_EQ(sequence.Size(), 2lu);
    EXPECT_EQ(sequence[0].As<int>(), 1);
    EXPECT_EQ(sequence[1].As<int>(), 3);

    sequence.SeqBatchPushBack(1, 3);
    LLBC_Variant removeSequence;
    removeSequence.SeqBatchPushBack(1, 3);
    sequence -= removeSequence;
    EXPECT_TRUE(sequence.IsEmpty());

    LLBC_Variant string(LLBC_String("404404"));
    string -= 404;
    EXPECT_EQ(string.As<LLBC_String>(), "");

    LLBC_Variant raw(404404);
    raw -= LLBC_String("404");
    EXPECT_TRUE(raw.Is<LLBC_Variant::Str>());
    EXPECT_EQ(raw.As<LLBC_String>(), "");
}

// Multiplication implements collection intersection/repetition. Division and
// modulo intentionally leave non-raw values alone, preserving structured data.
TEST(VariantOperationsTest, IntersectsRepeatsAndPreservesStructuredValues)
{
    LLBC_Variant::Dict dictionaryData;
    dictionaryData.emplace(LLBC_Variant(1), LLBC_Variant(10));
    dictionaryData.emplace(LLBC_Variant(2), LLBC_Variant(20));
    dictionaryData.emplace(LLBC_Variant(3), LLBC_Variant(30));
    LLBC_Variant dictionary(dictionaryData);
    LLBC_Variant dictionaryKeys;
    dictionaryKeys.SeqBatchPushBack(2, 3, 4);
    dictionary *= dictionaryKeys;
    EXPECT_EQ(dictionary.Size(), 2lu);
    EXPECT_TRUE(dictionary.Contains(2));
    EXPECT_TRUE(dictionary.Contains(3));

    LLBC_Variant emptySequence;
    emptySequence.Become<LLBC_Variant::Seq>();
    dictionary *= emptySequence;
    EXPECT_TRUE(dictionary.IsEmpty());

    LLBC_Variant sequence;
    sequence.SeqBatchPushBack(1, 2, 3);
    LLBC_Variant::Dict sequenceKeys;
    sequenceKeys.emplace(LLBC_Variant(2), LLBC_Variant());
    sequenceKeys.emplace(LLBC_Variant(3), LLBC_Variant());
    sequence *= LLBC_Variant(sequenceKeys);
    ASSERT_EQ(sequence.Size(), 2lu);
    EXPECT_EQ(sequence[0].As<int>(), 2);
    EXPECT_EQ(sequence[1].As<int>(), 3);

    sequence *= 3;
    EXPECT_EQ(sequence.Size(), 6lu);
    sequence *= 1;
    EXPECT_EQ(sequence.Size(), 6lu);
    sequence *= 0;
    EXPECT_TRUE(sequence.IsEmpty());

    LLBC_Variant text(LLBC_String("xy"));
    text *= 1;
    EXPECT_EQ(text.As<LLBC_String>(), "xy");
    text *= 0;
    EXPECT_EQ(text.As<LLBC_String>(), "");

    LLBC_Variant raw(9);
    raw *= LLBC_String("ignored");
    EXPECT_EQ(raw.As<int>(), 9);

    LLBC_Variant structured(LLBC_String("unchanged"));
    structured /= 2;
    structured %= 2;
    EXPECT_EQ(structured.As<LLBC_String>(), "unchanged");

    LLBC_Variant nil;
    nil /= 2;
    EXPECT_TRUE(nil.Is<void>());
    nil %= 2;
    EXPECT_TRUE(nil.Is<void>());

    EXPECT_EQ(LLBC_VariantTraits::sub(LLBC_Variant(8), LLBC_Variant(3)).As<int>(), 5);
    EXPECT_EQ(LLBC_VariantTraits::mul(LLBC_Variant(6), LLBC_Variant(7)).As<int>(), 42);
    EXPECT_EQ(LLBC_VariantTraits::div(LLBC_Variant(84), LLBC_Variant(2)).As<int>(), 42);
    EXPECT_EQ(LLBC_VariantTraits::mod(LLBC_Variant(85), LLBC_Variant(43)).As<int>(), 42);
}

// Empty inputs are common when layered configuration fragments are optional.
// The collection algebra must preserve no-op cases, clear completed removals,
// and compare structured values lexicographically through the same public
// operators used by Variant-backed maps and sets.
TEST(VariantOperationsTest, HandlesEmptyAlgebraAndStructuredComparisonEdges)
{
    LLBC_Variant emptyDictionary;
    emptyDictionary.Become<LLBC_Variant::Dict>();
    LLBC_Variant dictionary;
    dictionary["key"] = 1;
    dictionary += emptyDictionary;
    EXPECT_EQ(dictionary.Size(), 1lu);

    LLBC_Variant emptySequence;
    emptySequence.Become<LLBC_Variant::Seq>();
    LLBC_Variant sequence;
    sequence.SeqPushBack(1);
    sequence += emptyDictionary;
    sequence += emptySequence;
    EXPECT_EQ(sequence.As<LLBC_String>(), "[1]");

    LLBC_Variant emptyDictionarySubtraction;
    emptyDictionarySubtraction.Become<LLBC_Variant::Dict>();
    emptyDictionarySubtraction -= 1;
    EXPECT_TRUE(emptyDictionarySubtraction.IsEmpty());

    LLBC_Variant dictionaryToClear;
    dictionaryToClear["only"] = 1;
    LLBC_Variant dictionaryRemoval;
    dictionaryRemoval["only"] = 0;
    dictionaryToClear -= dictionaryRemoval;
    EXPECT_TRUE(dictionaryToClear.IsEmpty());

    LLBC_Variant emptySequenceSubtraction;
    emptySequenceSubtraction.Become<LLBC_Variant::Seq>();
    emptySequenceSubtraction -= 1;
    EXPECT_TRUE(emptySequenceSubtraction.IsEmpty());

    LLBC_Variant sequenceToClear;
    sequenceToClear.SeqPushBack(LLBC_String("only"));
    sequenceToClear -= dictionaryRemoval;
    EXPECT_TRUE(sequenceToClear.IsEmpty());

    LLBC_Variant emptyString(LLBC_String(""));
    emptyString -= LLBC_String("ignored");
    EXPECT_TRUE(emptyString.IsEmpty());
    LLBC_Variant unchangedString(LLBC_String("content"));
    unchangedString -= LLBC_String("");
    EXPECT_EQ(unchangedString.As<LLBC_String>(), "content");

    LLBC_Variant emptyDictionaryIntersection;
    emptyDictionaryIntersection.Become<LLBC_Variant::Dict>();
    emptyDictionaryIntersection *= 1;
    EXPECT_TRUE(emptyDictionaryIntersection.IsEmpty());
    LLBC_Variant emptySequenceIntersection;
    emptySequenceIntersection.Become<LLBC_Variant::Seq>();
    emptySequenceIntersection *= 2;
    EXPECT_TRUE(emptySequenceIntersection.IsEmpty());
    LLBC_Variant sequenceIntersection;
    sequenceIntersection.SeqPushBack(1);
    sequenceIntersection *= emptySequence;
    EXPECT_TRUE(sequenceIntersection.IsEmpty());
    LLBC_Variant emptyStringIntersection(LLBC_String(""));
    emptyStringIntersection *= 3;
    EXPECT_TRUE(emptyStringIntersection.IsEmpty());

    const LLBC_Variant nilLeft;
    const LLBC_Variant nilRight;
    EXPECT_TRUE(nilLeft == nilRight);
    EXPECT_FALSE(nilLeft < nilRight);
    EXPECT_TRUE(nilLeft < LLBC_Variant(0));
    EXPECT_FALSE(LLBC_Variant(0) < nilLeft);
    LLBC_Variant selfComparable(1);
    EXPECT_TRUE(selfComparable == selfComparable);
    EXPECT_FALSE(selfComparable < selfComparable);
    EXPECT_FALSE(LLBC_Variant(1) == LLBC_Variant(LLBC_String("1")));

    EXPECT_TRUE(LLBC_Variant(LLBC_String("same")) == LLBC_Variant(LLBC_String("same")));
    LLBC_Variant sequenceLeft;
    sequenceLeft.SeqPushBack(1);
    LLBC_Variant sequenceEqual = sequenceLeft;
    LLBC_Variant sequenceGreater;
    sequenceGreater.SeqPushBack(2);
    EXPECT_TRUE(sequenceLeft == sequenceEqual);
    EXPECT_TRUE(sequenceLeft < sequenceGreater);

    LLBC_Variant dictionaryLeft;
    dictionaryLeft["key"] = 1;
    LLBC_Variant dictionaryEqual = dictionaryLeft;
    LLBC_Variant dictionaryGreater;
    dictionaryGreater["key"] = 2;
    EXPECT_TRUE(dictionaryLeft == dictionaryEqual);
    EXPECT_TRUE(dictionaryLeft < dictionaryGreater);

    EXPECT_TRUE(LLBC_Variant(1.0f) == LLBC_Variant(1.0));
    EXPECT_TRUE(LLBC_Variant(static_cast<uint32>(1)) < LLBC_Variant(static_cast<uint64>(2)));
}

// Every supported raw type must route all arithmetic operations without
// crashing. Pointer values are treated as opaque integer-sized values and are
// never dereferenced by Variant arithmetic.
TEST(VariantOperationsTest, CoversSubMulDivAndModRawDispatchMatrix)
{
    const void *leftPointer = reinterpret_cast<void *>(static_cast<uintptr_t>(8));
    const void *rightPointer = reinterpret_cast<void *>(static_cast<uintptr_t>(2));
    const std::vector<std::pair<LLBC_Variant, LLBC_Variant>> values = {
        {LLBC_Variant(true), LLBC_Variant(true)},
        {LLBC_Variant(static_cast<sint8>(8)), LLBC_Variant(static_cast<sint8>(2))},
        {LLBC_Variant(static_cast<uint8>(8)), LLBC_Variant(static_cast<uint8>(2))},
        {LLBC_Variant(static_cast<sint16>(8)), LLBC_Variant(static_cast<sint16>(2))},
        {LLBC_Variant(static_cast<uint16>(8)), LLBC_Variant(static_cast<uint16>(2))},
        {LLBC_Variant(static_cast<sint32>(8)), LLBC_Variant(static_cast<sint32>(2))},
        {LLBC_Variant(static_cast<uint32>(8)), LLBC_Variant(static_cast<uint32>(2))},
        {LLBC_Variant(8L), LLBC_Variant(2L)},
        {LLBC_Variant(8UL), LLBC_Variant(2UL)},
        {LLBC_Variant(leftPointer), LLBC_Variant(rightPointer)},
        {LLBC_Variant(static_cast<sint64>(8)), LLBC_Variant(static_cast<sint64>(2))},
        {LLBC_Variant(static_cast<uint64>(8)), LLBC_Variant(static_cast<uint64>(2))},
        {LLBC_Variant(8.5f), LLBC_Variant(2.0f)},
        {LLBC_Variant(8.5), LLBC_Variant(2.0)},
    };

    for (const auto &pair : values)
    {
        LLBC_Variant subtraction(pair.first);
        subtraction -= pair.second;
        EXPECT_TRUE(subtraction.IsRaw());

        LLBC_Variant multiplication(pair.first);
        multiplication *= pair.second;
        EXPECT_TRUE(multiplication.IsRaw());

        LLBC_Variant division(pair.first);
        division /= pair.second;
        EXPECT_TRUE(division.IsRaw());

        LLBC_Variant modulo(pair.first);
        modulo %= pair.second;
        EXPECT_TRUE(modulo.IsRaw());
    }
}

// Relational operators consistently compare raw values, strings, sequences, and
// dictionaries; self-referential compound operations must clone safely first.
TEST(VariantOperationsTest, ComparesValuesAndHandlesSelfCompoundOperations)
{
    EXPECT_TRUE(LLBC_Variant(1) == LLBC_Variant(1u));
    EXPECT_TRUE(LLBC_Variant(1) < LLBC_Variant(2));
    EXPECT_TRUE(LLBC_Variant(2.0) >= LLBC_Variant(2));
    EXPECT_TRUE(LLBC_Variant(LLBC_String("abc")) < LLBC_Variant(LLBC_String("abd")));

    LLBC_Variant selfAdd(3);
    selfAdd += selfAdd;
    EXPECT_EQ(selfAdd.As<int>(), 6);

    LLBC_Variant selfSub(3);
    selfSub -= selfSub;
    EXPECT_EQ(selfSub.As<int>(), 0);

    LLBC_Variant selfMul(3);
    selfMul *= selfMul;
    EXPECT_EQ(selfMul.As<int>(), 9);

    LLBC_Variant selfDiv(3);
    selfDiv /= selfDiv;
    EXPECT_EQ(selfDiv.As<int>(), 1);

    LLBC_Variant selfMod(3);
    selfMod %= selfMod;
    EXPECT_EQ(selfMod.As<int>(), 0);
}

// Variant is the adapter between dynamic configuration values and ordinary STL
// containers. Verify pair, sequence, set, queue, ordered-map, and unordered-map
// conversions retain values while rvalue inputs transfer their owned elements.
TEST(VariantOperationsTest, AdaptsStandardContainersThroughCopyMoveAndConversion)
{
    using StringIntPair = std::pair<std::string, int>;
    using StringIntMap = std::map<std::string, int>;
    using StringIntUnorderedMap = std::unordered_map<std::string, int>;

    const StringIntPair initialPair {"left", 42};
    LLBC_Variant pairValue(std::make_pair(std::string("left"), 42));
    EXPECT_TRUE(pairValue.Is<LLBC_Variant::Seq>());
    EXPECT_EQ(pairValue.As<StringIntPair>(), initialPair);
    EXPECT_EQ(pairValue.As<const StringIntPair>(), initialPair);
    pairValue.SeqBatchPushBack("stale", 9, 10);
    pairValue = std::make_pair(std::string("right"), 7);
    const StringIntPair replacementPair {"right", 7};
    EXPECT_EQ(pairValue.Size(), 2lu);
    EXPECT_EQ(pairValue.As<StringIntPair>(), replacementPair);
    LLBC_Variant emptyPairAssignment;
    emptyPairAssignment.Become<LLBC_Variant::Seq>();
    emptyPairAssignment = std::make_pair(std::string("initial"), -1);
    EXPECT_EQ(emptyPairAssignment.As<StringIntPair>(),
              (StringIntPair {"initial", -1}));

    std::vector<int> copiedVector {1, 2, 3};
    LLBC_Variant sequenceValue(copiedVector);
    EXPECT_EQ(sequenceValue.As<std::deque<int>>(), std::deque<int>({1, 2, 3}));
    sequenceValue = std::vector<int> {4, 5};
    EXPECT_EQ(sequenceValue.As<std::list<int>>(), std::list<int>({4, 5}));
    sequenceValue = std::vector<int>();
    EXPECT_TRUE(sequenceValue.Is<LLBC_Variant::Seq>());
    EXPECT_TRUE(sequenceValue.IsEmpty());
    std::vector<int> movedVectorAssignment {12, 13};
    sequenceValue = std::move(movedVectorAssignment);
    EXPECT_TRUE(movedVectorAssignment.empty());
    EXPECT_EQ(sequenceValue.As<std::vector<int>>(), std::vector<int>({12, 13}));

    std::queue<int> movedQueue;
    movedQueue.push(6);
    movedQueue.push(7);
    LLBC_Variant queueValue(std::move(movedQueue));
    EXPECT_TRUE(movedQueue.empty());
    auto queueRoundTrip = queueValue.As<std::queue<int>>();
    ASSERT_FALSE(queueRoundTrip.empty());
    EXPECT_EQ(queueRoundTrip.front(), 6);
    queueRoundTrip.pop();
    ASSERT_FALSE(queueRoundTrip.empty());
    EXPECT_EQ(queueRoundTrip.front(), 7);

    const std::set<int> copiedSet {9, 3, 6};
    LLBC_Variant setValue(copiedSet);
    EXPECT_EQ(setValue.As<std::set<int>>(), copiedSet);

    std::unordered_set<int> movedSet {8, 4, 2};
    LLBC_Variant unorderedSetValue(std::move(movedSet));
    EXPECT_TRUE(movedSet.empty());
    EXPECT_EQ(unorderedSetValue.As<std::unordered_set<int>>(),
              std::unordered_set<int>({2, 4, 8}));
    std::unordered_set<int> replacementSet {1, 3};
    unorderedSetValue = std::move(replacementSet);
    EXPECT_TRUE(replacementSet.empty());
    EXPECT_EQ(unorderedSetValue.As<std::unordered_set<int>>(),
              std::unordered_set<int>({1, 3}));
    LLBC_Variant emptySetAssignment;
    emptySetAssignment.Become<LLBC_Variant::Seq>();
    std::unordered_set<int> initialSetAssignment {5, 10};
    emptySetAssignment = std::move(initialSetAssignment);
    EXPECT_TRUE(initialSetAssignment.empty());
    EXPECT_EQ(emptySetAssignment.As<std::unordered_set<int>>(),
              std::unordered_set<int>({5, 10}));

    StringIntMap movedMap {{"one", 1}, {"two", 2}};
    LLBC_Variant dictionaryValue(std::move(movedMap));
    EXPECT_TRUE(movedMap.empty());
    const StringIntMap expectedMap {{"one", 1}, {"two", 2}};
    EXPECT_EQ(dictionaryValue.As<StringIntMap>(), expectedMap);

    StringIntUnorderedMap movedUnorderedMap {{"alpha", 11}, {"beta", 22}};
    LLBC_Variant unorderedDictionaryValue(std::move(movedUnorderedMap));
    EXPECT_TRUE(movedUnorderedMap.empty());
    const StringIntUnorderedMap expectedUnorderedMap {
        {"alpha", 11},
        {"beta", 22},
    };
    EXPECT_EQ(unorderedDictionaryValue.As<StringIntUnorderedMap>(), expectedUnorderedMap);
    StringIntUnorderedMap replacementUnorderedMap {{"gamma", 33}};
    unorderedDictionaryValue = std::move(replacementUnorderedMap);
    EXPECT_TRUE(replacementUnorderedMap.empty());
    EXPECT_EQ(unorderedDictionaryValue.As<StringIntUnorderedMap>(),
              (StringIntUnorderedMap {{"gamma", 33}}));
}
