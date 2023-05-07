//===- unittest/Format/ObjCPropertyAttributeOrderFixerTest.cpp - unit tests
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "../lib/Format/ObjCPropertyAttributeOrderFixer.h"
#include "FormatTestBase.h"
#include "TestLexer.h"

#define DEBUG_TYPE "format-objc-property-attribute-order-fixer-test"

namespace clang {
namespace format {
namespace test {
namespace {

#define CHECK_PARSE(TEXT, FIELD, VALUE)                                        \
  EXPECT_NE(VALUE, Style.FIELD) << "Initial value already the same!";          \
  EXPECT_EQ(0, parseConfiguration(TEXT, &Style).value());                      \
  EXPECT_EQ(VALUE, Style.FIELD) << "Unexpected value after parsing!"

#define FAIL_PARSE(TEXT, FIELD, VALUE)                                         \
  EXPECT_NE(0, parseConfiguration(TEXT, &Style).value());                      \
  EXPECT_EQ(VALUE, Style.FIELD) << "Unexpected value after parsing!"

class ObjCPropertyAttributeOrderFixerTest : public FormatTestBase {
protected:
  TokenList annotate(llvm::StringRef Code,
                     const FormatStyle &Style = getLLVMStyle()) {
    return TestLexer(Allocator, Buffers, Style).annotate(Code);
  }
  llvm::SpecificBumpPtrAllocator<FormatToken> Allocator;
  std::vector<std::unique_ptr<llvm::MemoryBuffer>> Buffers;
};

TEST_F(ObjCPropertyAttributeOrderFixerTest, ParsesStyleOption) {
  FormatStyle Style = {};
  Style.Language = FormatStyle::LK_ObjC;

  CHECK_PARSE("ObjCPropertyAttributeOrder: [class]", ObjCPropertyAttributeOrder,
              std::vector<std::string>({"class"}));

  CHECK_PARSE("ObjCPropertyAttributeOrder: [direct, strong]",
              ObjCPropertyAttributeOrder,
              std::vector<std::string>({"direct", "strong"}));
}

TEST_F(ObjCPropertyAttributeOrderFixerTest, SortsSpecifiedAttributes) {
  FormatStyle Style = getLLVMStyle();
  Style.ObjCPropertyAttributeOrder = {"a", "b", "c"};

  verifyFormat("@property() int p;", Style);

  // One: shouldn't move.
  verifyFormat("@property(a) int p;", Style);
  verifyFormat("@property(b) int p;", Style);
  verifyFormat("@property(c) int p;", Style);

  // Two in correct order already: no change.
  verifyFormat("@property(a, b) int p;", Style);
  verifyFormat("@property(a, c) int p;", Style);
  verifyFormat("@property(b, c) int p;", Style);

  // Three in correct order already: no change.
  verifyFormat("@property(a, b, c) int p;", Style);

  // Two wrong order.
  verifyFormat("@property(a, b) int p;", "@property(b, a) int p;", Style);
  verifyFormat("@property(a, c) int p;", "@property(c, a) int p;", Style);
  verifyFormat("@property(b, c) int p;", "@property(c, b) int p;", Style);

  // Three wrong order.
  verifyFormat("@property(a, b, c) int p;", "@property(b, a, c) int p;", Style);
  verifyFormat("@property(a, b, c) int p;", "@property(c, b, a) int p;", Style);
}

TEST_F(ObjCPropertyAttributeOrderFixerTest, SortsAttributesWithValues) {
  FormatStyle Style = getLLVMStyle();
  Style.ObjCPropertyAttributeOrder = {"a", "getter", "c"};

  // No change
  verifyFormat("@property(getter=G, c) int p;", Style);
  verifyFormat("@property(a, getter=G) int p;", Style);
  verifyFormat("@property(a, getter=G, c) int p;", Style);

  // Reorder
  verifyFormat("@property(getter=G, c) int p;", "@property(c, getter=G) int p;",
               Style);
  verifyFormat("@property(a, getter=G) int p;", "@property(getter=G, a) int p;",
               Style);
  verifyFormat("@property(a, getter=G, c) int p;",
               "@property(getter=G, c, a) int p;", Style);

  // Multiple set properties, including ones not recognized
  verifyFormat("@property(a=A, c=C, x=X, y=Y) int p;",
               "@property(c=C, x=X, y=Y, a=A) int p;", Style);
}

TEST_F(ObjCPropertyAttributeOrderFixerTest, SortsUnspecifiedAttributesToBack) {
  FormatStyle Style = getLLVMStyle();
  Style.ObjCPropertyAttributeOrder = {"a", "b", "c"};

  verifyFormat("@property(x) int p;", Style);

  // No change in order.
  verifyFormat("@property(a, x, y) int p;", Style);
  verifyFormat("@property(b, x, y) int p;", Style);
  verifyFormat("@property(a, b, c, x, y) int p;", Style);

  // Reorder one unrecognized one.
  verifyFormat("@property(a, x) int p;", "@property(x, a) int p;", Style);

  // Prove the unrecognized ones have a stable sort order
  verifyFormat("@property(a, b, x, y) int p;", "@property(x, b, y, a) int p;",
               Style);
  verifyFormat("@property(a, b, y, x) int p;", "@property(y, b, x, a) int p;",
               Style);
}

TEST_F(ObjCPropertyAttributeOrderFixerTest, RemovesDuplicateAttributes) {
  FormatStyle Style = getLLVMStyle();
  Style.ObjCPropertyAttributeOrder = {"a", "b", "c"};

  verifyFormat("@property(a) int p;", "@property(a, a) int p;", Style);
  verifyFormat("@property(a) int p;", "@property(a, a, a, a) int p;", Style);

  verifyFormat("@property(a, b, c) int p;",
               "@property(c, b, a, b, a, c) int p;", Style);

  verifyFormat("@property(a, b, c, x, y) int p;",
               "@property(c, x, b, a, y, b, a, c, y) int p;", Style);
}

TEST_F(ObjCPropertyAttributeOrderFixerTest, HandlesClassAttribute) {
  // 'class' is the only attribute that is a keyword, so make sure it works too.
  FormatStyle Style = getLLVMStyle();
  Style.ObjCPropertyAttributeOrder = {"a", "class", "c"};

  // No change
  verifyFormat("@property(class, c) int p;", Style);
  verifyFormat("@property(a, class) int p;", Style);
  verifyFormat("@property(a, class, c) int p;", Style);

  // Reorder
  verifyFormat("@property(class, c) int p;", "@property(c, class) int p;",
               Style);
  verifyFormat("@property(a, class) int p;", "@property(class, a) int p;",
               Style);
  verifyFormat("@property(a, class, c) int p;", "@property(class, c, a) int p;",
               Style);
}

TEST_F(ObjCPropertyAttributeOrderFixerTest, HandlesCommentsAroundAttributes) {
  FormatStyle Style = getLLVMStyle();
  Style.ObjCPropertyAttributeOrder = {"a", "b", "c"};

  // Handle zero attributes but comments.
  verifyFormat("@property(/* 1 */) int p;", Style);
  verifyFormat("@property(/* 1 */ /* 2 */) int p;", Style);

  // Handle one attribute with comments before or after.
  verifyFormat("@property(/* 1 */ a) int p;", Style);
  verifyFormat("@property(a /* 2 */) int p;", Style);
  verifyFormat("@property(/* 1 */ a /* 2 */) int p;", Style);

  // Handle reordering with comments, before or after or both.
  verifyFormat("@property(/* 1 */ a, b, x, y) int p;",
               "@property(/* 1 */ x, b, a, y) int p;", Style);

  verifyFormat("@property(a, b, x, y /* 2 */) int p;",
               "@property(x, b, a, y /* 2 */) int p;", Style);

  verifyFormat("@property(/* 1 */ a, b, x, y /* 2 */) int p;",
               "@property(/* 1 */ x, b, a, y /* 2 */) int p;", Style);

  verifyFormat("@property(/* 1 */ /* 2 */ a, b, x, y /* 3 */ /* 4 */) int p;",
               "@property(/* 1 *//* 2 */ x,b,a,y /* 3 *//* 4 */) int p;",
               Style);

  // Comments between properties cause the pass to bail.
  verifyFormat("@property(a, /* 1 */ b) int p;", Style);
  verifyFormat("@property(b, /* 1 */ a) int p;", Style);
  verifyFormat("@property(b /* 1 */, a) int p;", Style);
}

} // namespace
} // namespace test
} // namespace format
} // namespace clang
