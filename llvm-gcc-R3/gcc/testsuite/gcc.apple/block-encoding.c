/* APPLE LOCAL file radar 5849129 */
/* Test for encoding of a block pointer as '@'. */
/* { dg-do compile } */
/* LLVM LOCAL add x86_64 Darwin */
/* { dg-options "-mmacosx-version-min=10.5 -ObjC" { target powerpc*-*-darwin* i?86*-*-darwin* x86_64-*-darwin* } } */
/* { dg-options "-iphoneos-version-min=4.0 -ObjC" { target arm*-*-darwin* } } */

#import <Foundation/Foundation.h>

@interface Test : NSObject
- (void)method1:(void *(^)(void *))a;
- (void)method2:(void *(^[5])(void *))b;
@end

@implementation Test
- (void)method1:(void *(^)(void *))a {}
- (void)method2:(void *(^[5])(void *))b {}
@end

int main() {
    return 0;
}
/* { dg-final { scan-assembler "v12@0:4@?" { target ilp32 } } } */
/* { dg-final { scan-assembler "v24@0:8@?" { target lp64 } } } */
/* { dg-final { scan-assembler "v12@0:4\[5@?\]" { target ilp32 } } } */
/* { dg-final { scan-assembler "v24@0:8\[5@?\]" { target lp64 } } } */
