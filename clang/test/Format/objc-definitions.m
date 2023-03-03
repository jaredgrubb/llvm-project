// RUN: grep -Ev "// *[A-Z-]+:" %s \
// RUN:   | clang-format -style="{BasedOnStyle: LLVM, AttributeMacros: [MACRO]}" \
// RUN:   | FileCheck -strict-whitespace %s --check-prefixes=CHECK-COMMON,CHECK-ATTRMACRO
// RUN: grep -Ev "// *[A-Z-]+:" %s \
// RUN:   | clang-format -style="{BasedOnStyle: LLVM, AttributeMacros: [MACRO], ColumnLimit: 0}" \
// RUN:   | FileCheck -strict-whitespace %s --check-prefixes=CHECK-COMMON,CHECK-ATTRMACRO
// RUN: grep -Ev "// *[A-Z-]+:" %s \
// RUN:   | clang-format -style="{BasedOnStyle: LLVM}" \
// RUN:   | FileCheck -strict-whitespace %s --check-prefixes=CHECK-COMMON,CHECK-PLAIN
// RUN: grep -Ev "// *[A-Z-]+:" %s \
// RUN:   | clang-format -style="{BasedOnStyle: LLVM, ColumnLimit: 0}" \
// RUN:   | FileCheck -strict-whitespace %s --check-prefixes=CHECK-COMMON,CHECK-PLAIN-COL0

// CHECK-COMMON: {{^@interface Foo$}}
// CHECK-COMMON-NEXT: {{^@end$}}
@interface Foo
@end

// CHECK-COMMON: {{^MACRO$}}
// CHECK-COMMON-NEXT: {{^@interface Foo$}}
// CHECK-COMMON-NEXT: {{^@end$}}
MACRO
@interface Foo
@end

// CHECK-COMMON: {{^MACRO\(A\)$}}
// CHECK-COMMON-NEXT: {{^@interface Foo$}}
// CHECK-COMMON-NEXT: {{^@end$}}
MACRO(A)
@interface Foo
@end

// CHECK-ATTRMACRO: {{^MACRO MACRO\(A\)$}}
// CHECK-ATTRMACRO-NEXT: {{^@interface Foo$}}
// CHECK-ATTRMACRO-NEXT: {{^@end$}}
// CHECK-PLAIN: {{^MACRO MACRO\(A\) @interface Foo$}}
// CHECK-PLAIN-NEXT: {{^@end$}}
// COM: The leading indentation in the next case is existing behavior but probably not desired.
// CHECK-PLAIN-COL0: {{^MACRO MACRO\(A\)$}}
// CHECK-PLAIN-COL0-NEXT: {{^    @interface Foo$}}
// CHECK-PLAIN-COL0-NEXT: {{^@end$}}
MACRO MACRO(A)
@interface Foo
@end

// CHECK-ATTRMACRO: {{^MACRO\(A\) MACRO$}}
// CHECK-ATTRMACRO-NEXT: {{^@interface Foo$}}
// CHECK-ATTRMACRO-NEXT: {{^@end$}}
// CHECK-PLAIN: {{^MACRO\(A\) MACRO @interface Foo$}}
// CHECK-PLAIN-NEXT: {{^@end$}}
// COM: The leading indentation in the next case is existing behavior but probably not desired.
// CHECK-PLAIN-COL0: {{^MACRO\(A\)$}}
// CHECK-PLAIN-COL0: {{^MACRO$}}
// CHECK-PLAIN-COL0-NEXT: {{^    @interface Foo$}}
// CHECK-PLAIN-COL0-NEXT: {{^@end$}}
MACRO(A) MACRO
@interface Foo
@end
