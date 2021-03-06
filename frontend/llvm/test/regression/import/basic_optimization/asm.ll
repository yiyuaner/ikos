; ModuleID = 'asm.c.pp.bc'
source_filename = "asm.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.12.0"

; CHECK-LABEL: Bundle
; CHECK: target-endianness = little-endian
; CHECK: target-pointer-size = 64 bits
; CHECK: target-triple = x86_64-apple-macosx10.12.0

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
; CHECK: define [4 x si8]* @.str, align 1, init {
; CHECK: #1 !entry !exit {
; CHECK:   store @.str, [37, 100, 10, 0], align 1
; CHECK: }
; CHECK: }

declare i32 @printf(i8*, ...) #1
; CHECK: declare si32 @ar.libc.printf(si8*, ...)

; Function Attrs: nounwind readnone
declare void @llvm.dbg.value(metadata, i64, metadata, metadata) #2

; Function Attrs: noinline nounwind ssp uwtable
define i32 @main() #0 !dbg !7 {
  call void @llvm.dbg.value(metadata i32 1, i64 0, metadata !11, metadata !12), !dbg !13
  %1 = call i32 asm "mov $1, $0\0A\09add $$1, $0", "=r,r,~{dirflag},~{fpsr},~{flags}"(i32 1) #2, !dbg !14, !srcloc !15
  call void @llvm.dbg.value(metadata i32 %1, i64 0, metadata !16, metadata !12), !dbg !17
  %2 = getelementptr inbounds [4 x i8], [4 x i8]* @.str, i32 0, i32 0, !dbg !18
  %3 = call i32 (i8*, ...) @printf(i8* %2, i32 %1), !dbg !18
  ret i32 %1, !dbg !19
}
; CHECK: define si32 @main() {
; CHECK: #1 !entry !exit {
; CHECK:   si32 %1 = call asm "mov $1, $0
; CHECK: 	add $$1, $0"(1)
; CHECK:   si8* %2 = ptrshift @.str, 4 * 0, 1 * 0
; CHECK:   si32 %3 = call @ar.libc.printf(%2, %1)
; CHECK:   return %1
; CHECK: }
; CHECK: }

attributes #0 = { noinline nounwind ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind readnone }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4, !5}
!llvm.ident = !{!6}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 4.0.1 (tags/RELEASE_401/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2)
!1 = !DIFile(filename: "asm.c", directory: "/Users/marthaud/ikos/ikos-git/frontend/llvm/test/regression/import/normal_optimization")
!2 = !{}
!3 = !{i32 2, !"Dwarf Version", i32 4}
!4 = !{i32 2, !"Debug Info Version", i32 3}
!5 = !{i32 1, !"PIC Level", i32 2}
!6 = !{!"clang version 4.0.1 (tags/RELEASE_401/final)"}
!7 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 3, type: !8, isLocal: false, isDefinition: true, scopeLine: 3, isOptimized: false, unit: !0, variables: !2)
!8 = !DISubroutineType(types: !9)
!9 = !{!10}
!10 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!11 = !DILocalVariable(name: "src", scope: !7, file: !1, line: 4, type: !10)
!12 = !DIExpression()
!13 = !DILocation(line: 4, column: 7, scope: !7)
!14 = !DILocation(line: 7, column: 3, scope: !7)
!15 = !{i32 68, i32 81}
!16 = !DILocalVariable(name: "dst", scope: !7, file: !1, line: 5, type: !10)
!17 = !DILocation(line: 5, column: 7, scope: !7)
!18 = !DILocation(line: 12, column: 3, scope: !7)
!19 = !DILocation(line: 13, column: 3, scope: !7)
