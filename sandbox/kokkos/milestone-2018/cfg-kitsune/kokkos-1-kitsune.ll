; ModuleID = 'forall-kokkos-1.cpp'
source_filename = "forall-kokkos-1.cpp"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%"class.std::basic_ostream" = type { i32 (...)**, %"class.std::basic_ios" }
%"class.std::basic_ios" = type { %"class.std::ios_base", %"class.std::basic_ostream"*, i8, i8, %"class.std::basic_streambuf"*, %"class.std::ctype"*, %"class.std::num_put"*, %"class.std::num_get"* }
%"class.std::ios_base" = type { i32 (...)**, i64, i64, i32, i32, i32, %"struct.std::ios_base::_Callback_list"*, %"struct.std::ios_base::_Words", [8 x %"struct.std::ios_base::_Words"], i32, %"struct.std::ios_base::_Words"*, %"class.std::locale" }
%"struct.std::ios_base::_Callback_list" = type { %"struct.std::ios_base::_Callback_list"*, void (i32, %"class.std::ios_base"*, i32)*, i32, i32 }
%"struct.std::ios_base::_Words" = type { i8*, i64 }
%"class.std::locale" = type { %"class.std::locale::_Impl"* }
%"class.std::locale::_Impl" = type { i32, %"class.std::locale::facet"**, i64, %"class.std::locale::facet"**, i8** }
%"class.std::locale::facet" = type <{ i32 (...)**, i32, [4 x i8] }>
%"class.std::basic_streambuf" = type { i32 (...)**, i8*, i8*, i8*, i8*, i8*, i8*, %"class.std::locale" }
%"class.std::ctype" = type <{ %"class.std::locale::facet.base", [4 x i8], %struct.__locale_struct*, i8, [7 x i8], i32*, i32*, i16*, i8, [256 x i8], [256 x i8], i8, [6 x i8] }>
%"class.std::locale::facet.base" = type <{ i32 (...)**, i32 }>
%struct.__locale_struct = type { [13 x %struct.__locale_data*], i16*, i32*, i32*, [13 x i8*] }
%struct.__locale_data = type opaque
%"class.std::num_put" = type { %"class.std::locale::facet.base", [4 x i8] }
%"class.std::num_get" = type { %"class.std::locale::facet.base", [4 x i8] }
%"class.std::ios_base::Init" = type { i8 }
%"class.Kokkos::Impl::OpenMPExec" = type { i32, i32, [512 x %"class.Kokkos::Impl::HostThreadTeamData"*] }
%"class.Kokkos::Impl::HostThreadTeamData" = type { %"struct.Kokkos::pair", i64, i64*, i64*, i64*, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32 }
%"struct.Kokkos::pair" = type { i64, i64 }
%ident_t = type { i32, i32, i32, i32, i8* }
%struct.__cilkrts_stack_frame = type { i32, i32, %struct.__cilkrts_stack_frame*, %struct.__cilkrts_worker*, i8*, [5 x i8*], i32, i16, i16, { %struct.__cilkrts_pedigree } }
%struct.__cilkrts_worker = type { %struct.__cilkrts_stack_frame**, %struct.__cilkrts_stack_frame**, %struct.__cilkrts_stack_frame**, %struct.__cilkrts_stack_frame**, %struct.__cilkrts_stack_frame**, i32, i8*, i8*, i8*, %struct.__cilkrts_stack_frame*, i8*, i8*, %struct.__cilkrts_pedigree }
%struct.__cilkrts_pedigree = type { i64, %struct.__cilkrts_pedigree* }
%"class.Kokkos::View" = type { %"union.Kokkos::Impl::SharedAllocationTracker", %"class.Kokkos::Impl::ViewMapping" }
%"union.Kokkos::Impl::SharedAllocationTracker" = type { i64 }
%"class.Kokkos::Impl::ViewMapping" = type { double*, %"struct.Kokkos::Impl::ViewOffset" }
%"struct.Kokkos::Impl::ViewOffset" = type { %"struct.Kokkos::Impl::ViewDimension", i64 }
%"struct.Kokkos::Impl::ViewDimension" = type { %"struct.Kokkos::Impl::ViewDimension0" }
%"struct.Kokkos::Impl::ViewDimension0" = type { i64 }
%"struct.Kokkos::Impl::ViewCtorProp" = type { %"struct.Kokkos::Impl::ViewCtorProp.0" }
%"struct.Kokkos::Impl::ViewCtorProp.0" = type { %"class.std::__cxx11::basic_string" }
%"class.std::__cxx11::basic_string" = type { %"struct.std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_Alloc_hider", i64, %union.anon }
%"struct.std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_Alloc_hider" = type { i8* }
%union.anon = type { i64, [8 x i8] }
%"struct.Kokkos::LayoutRight" = type { [8 x i64] }
%"class.Kokkos::Impl::SharedAllocationRecord" = type <{ i32 (...)**, %"class.Kokkos::Impl::SharedAllocationHeader"*, i64, void (%"class.Kokkos::Impl::SharedAllocationRecord"*)*, i32, [4 x i8] }>
%"class.Kokkos::Impl::SharedAllocationHeader" = type { %"class.Kokkos::Impl::SharedAllocationRecord"*, [120 x i8] }
%"class.std::allocator" = type { i8 }
%"struct.Kokkos::Impl::ViewCtorProp.1" = type <{ %"struct.Kokkos::Impl::ViewCtorProp.0", %"struct.Kokkos::Impl::ViewCtorProp.3", %"struct.Kokkos::Impl::ViewCtorProp.4", [3 x i8] }>
%"struct.Kokkos::Impl::ViewCtorProp.3" = type { %"class.Kokkos::HostSpace" }
%"class.Kokkos::HostSpace" = type { i32 }
%"struct.Kokkos::Impl::ViewCtorProp.4" = type { %"class.Kokkos::OpenMP" }
%"class.Kokkos::OpenMP" = type { i8 }
%"struct.Kokkos::ViewTraits" = type { i8 }
%"struct.Kokkos::Impl::ViewCtorProp.2" = type { i8 }
%"struct.std::integral_constant" = type { i8 }
%"class.Kokkos::Impl::SharedAllocationRecord.6" = type { %"class.Kokkos::Impl::SharedAllocationRecord.7", %"struct.Kokkos::Impl::ViewValueFunctor" }
%"class.Kokkos::Impl::SharedAllocationRecord.7" = type { %"class.Kokkos::Impl::SharedAllocationRecord.base", %"class.Kokkos::HostSpace" }
%"class.Kokkos::Impl::SharedAllocationRecord.base" = type <{ i32 (...)**, %"class.Kokkos::Impl::SharedAllocationHeader"*, i64, void (%"class.Kokkos::Impl::SharedAllocationRecord"*)*, i32 }>
%"struct.Kokkos::Impl::ViewValueFunctor" = type { %"class.Kokkos::OpenMP", double*, i64 }
%"class.Kokkos::Impl::ParallelFor" = type { %"class.Kokkos::Impl::OpenMPExec"*, %"struct.Kokkos::Impl::ViewValueFunctor", %"class.Kokkos::RangePolicy" }
%"class.Kokkos::RangePolicy" = type { %"class.Kokkos::OpenMP", i64, i64, i64, i64 }
%"struct.Kokkos::Impl::ViewDimension1" = type { i8 }
%"struct.Kokkos::Impl::ViewDimension2" = type { i8 }
%"struct.Kokkos::Impl::ViewDimension3" = type { i8 }
%"struct.Kokkos::Impl::ViewDimension4" = type { i8 }
%"struct.Kokkos::Impl::ViewDimension5" = type { i8 }
%"struct.Kokkos::Impl::ViewDimension6" = type { i8 }
%"struct.Kokkos::Impl::ViewDimension7" = type { i8 }
%"struct.Kokkos::Impl::PolicyTraits" = type { i8 }
%"struct.std::pair" = type { i64, i64 }

$_ZN6Kokkos4ViewIA2_PdJEEC2IA2_cEERKT_NSt9enable_ifIXsr6Kokkos4Impl13is_view_labelIS6_EE5valueEKmE4typeEmmmmmmm = comdat any

$_ZN6Kokkos4ViewIA2_PdJEED2Ev = comdat any

$_ZN6Kokkos4Impl12ViewCtorPropIJNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEEEC2IJA2_cEEEDpRKT_ = comdat any

$_ZN6Kokkos11LayoutRightC2Emmmmmmmm = comdat any

$_ZN6Kokkos4ViewIA2_PdJEEC2IJNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEEEERKNS_4Impl12ViewCtorPropIJDpT_EEERKNSt9enable_ifIXntsr4Impl12ViewCtorPropISE_EE11has_pointerENS_11LayoutRightEE4typeE = comdat any

$_ZN6Kokkos4Impl12ViewCtorPropIJNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEEED2Ev = comdat any

$_ZN6Kokkos4Impl23runtime_check_rank_hostEmbmmmmmmmmRKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE = comdat any

$_ZNK6Kokkos4ViewIA2_PdJEE5labelB5cxx11Ev = comdat any

$_ZN6Kokkos4Impl12ViewCtorPropIJvNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEEEC2EOS7_ = comdat any

$_ZN6Kokkos4Impl11ViewMappingINS_10ViewTraitsIA2_PdJEEEJvEEC2Ev = comdat any

$_ZN6Kokkos6OpenMP14is_initializedEv = comdat any

$_ZN6Kokkos4Impl12ViewCtorPropIJNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEESt17integral_constantIjLj0EENS_9HostSpaceENS_6OpenMPEEEC2IJS7_EEERKNS1_IJDpT_EEE = comdat any

$_ZN6Kokkos4Impl11ViewMappingINS_10ViewTraitsIA2_PdJEEEJvEE15allocate_sharedIJNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEESt17integral_constantIjLj0EENS_9HostSpaceENS_6OpenMPEEEEPNS0_22SharedAllocationRecordIvvEERKNS0_12ViewCtorPropIJDpT_EEERKNS_11LayoutRightE = comdat any

$_ZN6Kokkos4Impl23SharedAllocationTracker40assign_allocated_record_to_uninitializedEPNS0_22SharedAllocationRecordIvvEE = comdat any

$_ZN6Kokkos4Impl12ViewCtorPropIJNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEESt17integral_constantIjLj0EENS_9HostSpaceENS_6OpenMPEEED2Ev = comdat any

$_ZN6Kokkos4Impl11ViewMappingINS_10ViewTraitsIA2_PdJEEEJvEED2Ev = comdat any

$_ZN6Kokkos4Impl12ViewCtorPropIJvNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEEEC2ERKS8_ = comdat any

$_ZN6Kokkos4Impl12ViewCtorPropIJvNS_9HostSpaceEEEC2Ev = comdat any

$_ZN6Kokkos4Impl12ViewCtorPropIJvNS_6OpenMPEEEC2Ev = comdat any

$_ZN6Kokkos4Impl12ViewCtorPropIJvNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEEED2Ev = comdat any

$_ZN6Kokkos6OpenMPC2Ev = comdat any

$_ZN6Kokkos4Impl10ViewOffsetINS0_13ViewDimensionIJLm0ELm2EEEENS_11LayoutRightEvEC2ILj0EEERKSt17integral_constantIjXT_EERKS4_ = comdat any

$_ZNK6Kokkos4Impl10ViewOffsetINS0_13ViewDimensionIJLm0ELm2EEEENS_11LayoutRightEvE4spanEv = comdat any

$_ZN6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceENS0_16ViewValueFunctorINS_6OpenMPEdLb1EEEE8allocateERKS2_RKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEm = comdat any

$_ZNK6Kokkos4Impl22SharedAllocationRecordIvvE4dataEv = comdat any

$_ZN6Kokkos4Impl16ViewValueFunctorINS_6OpenMPEdLb1EEC2ERKS2_Pdm = comdat any

$_ZN6Kokkos4Impl16ViewValueFunctorINS_6OpenMPEdLb1EE27construct_shared_allocationEv = comdat any

$_ZN6Kokkos4Impl13ViewDimensionIJLm0ELm2EEEC2Emmmmmmmm = comdat any

$_ZN6Kokkos4Impl10ViewOffsetINS0_13ViewDimensionIJLm0ELm2EEEENS_11LayoutRightEvE7PaddingILj0EE6strideEm = comdat any

$_ZN6Kokkos4Impl14ViewDimension0ILm0ELj1EEC2Em = comdat any

$_ZN6Kokkos4Impl14ViewDimension1ILm2ELj1EEC2Em = comdat any

$_ZN6Kokkos4Impl14ViewDimension2ILm18446744073709551615ELj1EEC2Em = comdat any

$_ZN6Kokkos4Impl14ViewDimension3ILm18446744073709551615ELj1EEC2Em = comdat any

$_ZN6Kokkos4Impl14ViewDimension4ILm18446744073709551615ELj1EEC2Em = comdat any

$_ZN6Kokkos4Impl14ViewDimension5ILm18446744073709551615ELj1EEC2Em = comdat any

$_ZN6Kokkos4Impl14ViewDimension6ILm18446744073709551615ELj1EEC2Em = comdat any

$_ZN6Kokkos4Impl14ViewDimension7ILm18446744073709551615ELj1EEC2Em = comdat any

$_ZN6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceENS0_16ViewValueFunctorINS_6OpenMPEdLb1EEEEC2ERKS2_RKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEm = comdat any

$_ZN6Kokkos4Impl16ViewValueFunctorINS_6OpenMPEdLb1EEC2Ev = comdat any

$_ZNK6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceEvE9get_labelB5cxx11Ev = comdat any

$_ZN6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceENS0_16ViewValueFunctorINS_6OpenMPEdLb1EEEED2Ev = comdat any

$_ZN6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceENS0_16ViewValueFunctorINS_6OpenMPEdLb1EEEED0Ev = comdat any

$_ZN6Kokkos4Impl16ViewValueFunctorINS_6OpenMPEdLb1EE25destroy_shared_allocationEv = comdat any

$_ZNK6Kokkos4Impl22SharedAllocationRecordIvvE4headEv = comdat any

$_ZN6Kokkos6OpenMP11in_parallelERKS0_ = comdat any

$_ZN6Kokkos11RangePolicyIJNS_6OpenMPEEEC2Emm = comdat any

$_ZN6Kokkos4Impl11ParallelForINS0_16ViewValueFunctorINS_6OpenMPEdLb1EEENS_11RangePolicyIJS3_EEES3_EC2ERKS4_S6_ = comdat any

$_ZNK6Kokkos4Impl11ParallelForINS0_16ViewValueFunctorINS_6OpenMPEdLb1EEENS_11RangePolicyIJS3_EEES3_E7executeEv = comdat any

$_ZN6Kokkos6OpenMP5fenceERKS0_ = comdat any

$_ZNK6Kokkos4Impl16ViewValueFunctorINS_6OpenMPEdLb1EEclEm = comdat any

$__clang_call_terminate = comdat any

$_ZN6Kokkos11RangePolicyIJNS_6OpenMPEEEC2ERKS1_mm = comdat any

$_ZN6Kokkos11RangePolicyIJNS_6OpenMPEEE19set_auto_chunk_sizeEv = comdat any

$_ZN6Kokkos4Impl24is_integral_power_of_twoEm = comdat any

$_ZN6Kokkos5abortEPKc = comdat any

$_ZN6Kokkos4Impl11ParallelForINS0_16ViewValueFunctorINS_6OpenMPEdLb1EEENS_11RangePolicyIJS3_EEES3_E10exec_rangeIvEENSt9enable_ifIXsr3std7is_sameIT_vEE5valueEvE4typeERKS4_mm = comdat any

$_ZNK6Kokkos11RangePolicyIJNS_6OpenMPEEE5beginEv = comdat any

$_ZNK6Kokkos11RangePolicyIJNS_6OpenMPEEE3endEv = comdat any

$_ZN6Kokkos6OpenMP16thread_pool_sizeEv = comdat any

$_ZNK6Kokkos4Impl10OpenMPExec15get_thread_dataEv = comdat any

$_ZN6Kokkos4Impl18HostThreadTeamData18set_work_partitionEli = comdat any

$_ZNK6Kokkos11RangePolicyIJNS_6OpenMPEEE10chunk_sizeEv = comdat any

$_ZNSt4pairIllEC2IiiLb1EEEOT_OT0_ = comdat any

$_ZN6Kokkos4Impl18HostThreadTeamData23get_work_stealing_chunkEv = comdat any

$_ZN6Kokkos4Impl18HostThreadTeamData18get_work_partitionEv = comdat any

$_ZNSt4pairIllEaSEOS0_ = comdat any

$_ZNSt14numeric_limitsIiE3maxEv = comdat any

$_ZSt3maxIiERKT_S2_S2_ = comdat any

$_ZSt7forwardIiEOT_RNSt16remove_referenceIS0_E4typeE = comdat any

$_ZNSt4pairIllEC2IRlS2_Lb1EEEOT_OT0_ = comdat any

$_ZSt7forwardIRlEOT_RNSt16remove_referenceIS1_E4typeE = comdat any

$_ZSt7forwardIlEOT_RNSt16remove_referenceIS0_E4typeE = comdat any

$_ZNK6Kokkos4Impl23SharedAllocationTracker9get_labelINS_9HostSpaceEEENSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEv = comdat any

$_ZTVN6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceENS0_16ViewValueFunctorINS_6OpenMPEdLb1EEEEE = comdat any

$_ZTSN6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceENS0_16ViewValueFunctorINS_6OpenMPEdLb1EEEEE = comdat any

$_ZTIN6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceENS0_16ViewValueFunctorINS_6OpenMPEdLb1EEEEE = comdat any

@.str = private unnamed_addr constant [2 x i8] c"A\00", align 1
@_ZSt4cout = external global %"class.std::basic_ostream", align 8
@.str.1 = private unnamed_addr constant [3 x i8] c"a(\00", align 1
@.str.2 = private unnamed_addr constant [8 x i8] c", 0) = \00", align 1
@.str.3 = private unnamed_addr constant [8 x i8] c", 1) = \00", align 1
@_ZStL8__ioinit = internal global %"class.std::ios_base::Init" zeroinitializer, align 1
@__dso_handle = external hidden global i8
@.str.4 = private unnamed_addr constant [75 x i8] c"Constructing View and initializing data with uninitialized execution space\00", align 1
@_ZN6Kokkos4Impl17t_openmp_instanceE = external thread_local global %"class.Kokkos::Impl::OpenMPExec"*, align 8
@_ZTVN6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceENS0_16ViewValueFunctorINS_6OpenMPEdLb1EEEEE = linkonce_odr unnamed_addr constant { [5 x i8*] } { [5 x i8*] [i8* null, i8* bitcast ({ i8*, i8*, i8* }* @_ZTIN6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceENS0_16ViewValueFunctorINS_6OpenMPEdLb1EEEEE to i8*), i8* bitcast (void (%"class.std::__cxx11::basic_string"*, %"class.Kokkos::Impl::SharedAllocationRecord.7"*)* @_ZNK6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceEvE9get_labelB5cxx11Ev to i8*), i8* bitcast (void (%"class.Kokkos::Impl::SharedAllocationRecord.6"*)* @_ZN6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceENS0_16ViewValueFunctorINS_6OpenMPEdLb1EEEED2Ev to i8*), i8* bitcast (void (%"class.Kokkos::Impl::SharedAllocationRecord.6"*)* @_ZN6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceENS0_16ViewValueFunctorINS_6OpenMPEdLb1EEEED0Ev to i8*)] }, comdat, align 8
@_ZTVN10__cxxabiv120__si_class_type_infoE = external global i8*
@_ZTSN6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceENS0_16ViewValueFunctorINS_6OpenMPEdLb1EEEEE = linkonce_odr constant [96 x i8] c"N6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceENS0_16ViewValueFunctorINS_6OpenMPEdLb1EEEEE\00", comdat
@_ZTIN6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceEvEE = external constant i8*
@_ZTIN6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceENS0_16ViewValueFunctorINS_6OpenMPEdLb1EEEEE = linkonce_odr constant { i8*, i8*, i8* } { i8* bitcast (i8** getelementptr inbounds (i8*, i8** @_ZTVN10__cxxabiv120__si_class_type_infoE, i64 2) to i8*), i8* getelementptr inbounds ([96 x i8], [96 x i8]* @_ZTSN6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceENS0_16ViewValueFunctorINS_6OpenMPEdLb1EEEEE, i32 0, i32 0), i8* bitcast (i8** @_ZTIN6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceEvEE to i8*) }, comdat
@.str.5 = private unnamed_addr constant [29 x i8] c"Kokkos::View::initialization\00", align 1
@.str.6 = private unnamed_addr constant [54 x i8] c"RangePolicy blocking granularity must be power of two\00", align 1
@.str.7 = private unnamed_addr constant [28 x i8] c"Kokkos::OpenMP parallel_for\00", align 1
@.str.8 = private unnamed_addr constant [23 x i8] c";unknown;unknown;0;0;;\00", align 1
@0 = private unnamed_addr constant %ident_t { i32 0, i32 2, i32 0, i32 0, i8* getelementptr inbounds ([23 x i8], [23 x i8]* @.str.8, i32 0, i32 0) }, align 8
@llvm.global_ctors = appending global [1 x { i32, void ()*, i8* }] [{ i32, void ()*, i8* } { i32 65535, void ()* @_GLOBAL__sub_I_forall_kokkos_1.cpp, i8* null }]

; Function Attrs: noinline norecurse optnone uwtable
define i32 @main(i32 %argc, i8** %argv) #0 personality i8* bitcast (i32 (...)* @__gxx_personality_v0 to i8*) {
entry:
  %__cilkrts_sf = alloca %struct.__cilkrts_stack_frame, align 8
  %0 = call %struct.__cilkrts_worker* @__cilkrts_get_tls_worker() #7
  %1 = icmp eq %struct.__cilkrts_worker* %0, null
  br i1 %1, label %slowpath.i, label %fastpath.i

slowpath.i:                                       ; preds = %entry
  %2 = call %struct.__cilkrts_worker* @__cilkrts_bind_thread_1() #7
  %3 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 0
  store volatile i32 16777344, i32* %3
  br label %__cilkrts_enter_frame_1.exit

fastpath.i:                                       ; preds = %entry
  %4 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 0
  store volatile i32 16777216, i32* %4
  br label %__cilkrts_enter_frame_1.exit

__cilkrts_enter_frame_1.exit:                     ; preds = %slowpath.i, %fastpath.i
  %5 = phi %struct.__cilkrts_worker* [ %2, %slowpath.i ], [ %0, %fastpath.i ]
  %6 = getelementptr inbounds %struct.__cilkrts_worker, %struct.__cilkrts_worker* %5, i32 0, i32 9
  %7 = load volatile %struct.__cilkrts_stack_frame*, %struct.__cilkrts_stack_frame** %6
  %8 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 2
  store volatile %struct.__cilkrts_stack_frame* %7, %struct.__cilkrts_stack_frame** %8
  %9 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 3
  store volatile %struct.__cilkrts_worker* %5, %struct.__cilkrts_worker** %9
  %10 = getelementptr inbounds %struct.__cilkrts_worker, %struct.__cilkrts_worker* %5, i32 0, i32 9
  store volatile %struct.__cilkrts_stack_frame* %__cilkrts_sf, %struct.__cilkrts_stack_frame** %10
  %this.addr.i91 = alloca %"class.Kokkos::View"*, align 8
  %i0.addr.i92 = alloca i64*, align 8
  %i1.addr.i93 = alloca i32*, align 8
  %this.addr.i78 = alloca %"class.Kokkos::View"*, align 8
  %i0.addr.i79 = alloca i64*, align 8
  %i1.addr.i80 = alloca i32*, align 8
  %this.addr.i52 = alloca %"class.Kokkos::View"*, align 8
  %i0.addr.i53 = alloca i64*, align 8
  %i1.addr.i54 = alloca i32*, align 8
  %this.addr.i = alloca %"class.Kokkos::View"*, align 8
  %i0.addr.i = alloca i64*, align 8
  %i1.addr.i = alloca i32*, align 8
  %retval = alloca i32, align 4
  %argc.addr = alloca i32, align 4
  %argv.addr = alloca i8**, align 8
  %a = alloca %"class.Kokkos::View", align 8
  %i = alloca i64, align 8
  %ref.tmp = alloca i32, align 4
  %exn.slot = alloca i8*
  %ehselector.slot = alloca i32
  %ref.tmp1 = alloca i32, align 4
  %syncreg = call token @llvm.syncregion.start()
  %i4 = alloca i32, align 4
  %i17 = alloca i64, align 8
  %ai0 = alloca double, align 8
  %ref.tmp21 = alloca i32, align 4
  %ai1 = alloca double, align 8
  %ref.tmp24 = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  store i32 %argc, i32* %argc.addr, align 4
  store i8** %argv, i8*** %argv.addr, align 8
  %11 = load i8**, i8*** %argv.addr, align 8
  call void @_ZN6Kokkos10initializeERiPPc(i32* dereferenceable(4) %argc.addr, i8** %11)
  call void @_ZN6Kokkos4ViewIA2_PdJEEC2IA2_cEERKT_NSt9enable_ifIXsr6Kokkos4Impl13is_view_labelIS6_EE5valueEKmE4typeEmmmmmmm(%"class.Kokkos::View"* %a, [2 x i8]* dereferenceable(2) @.str, i64 1073741824, i64 0, i64 0, i64 0, i64 0, i64 0, i64 0, i64 0)
  store i64 0, i64* %i, align 8
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %__cilkrts_enter_frame_1.exit
  %12 = load i64, i64* %i, align 8
  %cmp = icmp ult i64 %12, 1073741824
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  store i32 0, i32* %ref.tmp, align 4
  store %"class.Kokkos::View"* %a, %"class.Kokkos::View"** %this.addr.i, align 8
  store i64* %i, i64** %i0.addr.i, align 8
  store i32* %ref.tmp, i32** %i1.addr.i, align 8
  %this1.i = load %"class.Kokkos::View"*, %"class.Kokkos::View"** %this.addr.i, align 8
  %m_map.i = getelementptr inbounds %"class.Kokkos::View", %"class.Kokkos::View"* %this1.i, i32 0, i32 1
  %m_handle.i = getelementptr inbounds %"class.Kokkos::Impl::ViewMapping", %"class.Kokkos::Impl::ViewMapping"* %m_map.i, i32 0, i32 0
  %13 = load double*, double** %m_handle.i, align 8
  %14 = load i32*, i32** %i1.addr.i, align 8
  %15 = load i32, i32* %14, align 4
  %conv.i = sext i32 %15 to i64
  %m_map2.i = getelementptr inbounds %"class.Kokkos::View", %"class.Kokkos::View"* %this1.i, i32 0, i32 1
  %m_offset.i = getelementptr inbounds %"class.Kokkos::Impl::ViewMapping", %"class.Kokkos::Impl::ViewMapping"* %m_map2.i, i32 0, i32 1
  %m_stride.i = getelementptr inbounds %"struct.Kokkos::Impl::ViewOffset", %"struct.Kokkos::Impl::ViewOffset"* %m_offset.i, i32 0, i32 1
  %16 = load i64, i64* %m_stride.i, align 8
  %17 = load i64*, i64** %i0.addr.i, align 8
  %18 = load i64, i64* %17, align 8
  %mul.i = mul i64 %16, %18
  %add.i = add i64 %conv.i, %mul.i
  %arrayidx.i = getelementptr inbounds double, double* %13, i64 %add.i
  br label %invoke.cont

invoke.cont:                                      ; preds = %for.body
  store double 0.000000e+00, double* %arrayidx.i, align 8
  store i32 1, i32* %ref.tmp1, align 4
  store %"class.Kokkos::View"* %a, %"class.Kokkos::View"** %this.addr.i52, align 8
  store i64* %i, i64** %i0.addr.i53, align 8
  store i32* %ref.tmp1, i32** %i1.addr.i54, align 8
  %this1.i55 = load %"class.Kokkos::View"*, %"class.Kokkos::View"** %this.addr.i52, align 8
  %m_map.i56 = getelementptr inbounds %"class.Kokkos::View", %"class.Kokkos::View"* %this1.i55, i32 0, i32 1
  %m_handle.i57 = getelementptr inbounds %"class.Kokkos::Impl::ViewMapping", %"class.Kokkos::Impl::ViewMapping"* %m_map.i56, i32 0, i32 0
  %19 = load double*, double** %m_handle.i57, align 8
  %20 = load i32*, i32** %i1.addr.i54, align 8
  %21 = load i32, i32* %20, align 4
  %conv.i58 = sext i32 %21 to i64
  %m_map2.i59 = getelementptr inbounds %"class.Kokkos::View", %"class.Kokkos::View"* %this1.i55, i32 0, i32 1
  %m_offset.i60 = getelementptr inbounds %"class.Kokkos::Impl::ViewMapping", %"class.Kokkos::Impl::ViewMapping"* %m_map2.i59, i32 0, i32 1
  %m_stride.i61 = getelementptr inbounds %"struct.Kokkos::Impl::ViewOffset", %"struct.Kokkos::Impl::ViewOffset"* %m_offset.i60, i32 0, i32 1
  %22 = load i64, i64* %m_stride.i61, align 8
  %23 = load i64*, i64** %i0.addr.i53, align 8
  %24 = load i64, i64* %23, align 8
  %mul.i62 = mul i64 %22, %24
  %add.i63 = add i64 %conv.i58, %mul.i62
  %arrayidx.i64 = getelementptr inbounds double, double* %19, i64 %add.i63
  br label %invoke.cont2

invoke.cont2:                                     ; preds = %invoke.cont
  store double 0.000000e+00, double* %arrayidx.i64, align 8
  br label %for.inc

for.inc:                                          ; preds = %invoke.cont2
  %25 = load i64, i64* %i, align 8
  %inc = add i64 %25, 1
  store i64 %inc, i64* %i, align 8
  br label %for.cond

lpad:                                             ; preds = %for.end49, %invoke.cont43, %invoke.cont41, %invoke.cont39, %invoke.cont37, %invoke.cont35, %invoke.cont33, %invoke.cont31, %invoke.cont29, %invoke.cont27, %invoke.cont25
  %26 = landingpad { i8*, i32 }
          cleanup
  %27 = extractvalue { i8*, i32 } %26, 0
  store i8* %27, i8** %exn.slot, align 8
  %28 = extractvalue { i8*, i32 } %26, 1
  store i32 %28, i32* %ehselector.slot, align 4
  br label %ehcleanup

for.end:                                          ; preds = %for.cond
  store i32 0, i32* %i4, align 4
  br label %kokkos.forall.cond

kokkos.forall.cond:                               ; preds = %kokkos.forall.inc, %for.end
  %29 = load i32, i32* %i4, align 4
  %30 = icmp ult i32 %29, 1073741824
  br i1 %30, label %kokkos.forall.detach, label %kokkos.forall.end

kokkos.forall.detach:                             ; preds = %kokkos.forall.cond
  %31 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 6
  %32 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 7
  call void asm sideeffect "stmxcsr $0\0A\09fnstcw $1", "*m,*m,~{dirflag},~{fpsr},~{flags}"(i32* %31, i16* %32)
  %33 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 5
  %34 = call i8* @llvm.frameaddress(i32 0)
  %35 = getelementptr inbounds [5 x i8*], [5 x i8*]* %33, i32 0, i32 0
  store volatile i8* %34, i8** %35
  %36 = call i8* @llvm.stacksave()
  %37 = getelementptr inbounds [5 x i8*], [5 x i8*]* %33, i32 0, i32 2
  store volatile i8* %36, i8** %37
  %38 = bitcast [5 x i8*]* %33 to i8*
  %39 = call i32 @llvm.eh.sjlj.setjmp(i8* %38) #12
  %40 = icmp eq i32 %39, 0
  br i1 %40, label %kokkos.forall.detach.split, label %kokkos.forall.detach.split.split

kokkos.forall.detach.split:                       ; preds = %kokkos.forall.detach
  call fastcc void @main_kokkos.forall.body.entry.cilk(i32* %i4, %"class.Kokkos::View"* %a)
  br label %kokkos.forall.detach.split.split

kokkos.forall.detach.split.split:                 ; preds = %kokkos.forall.detach, %kokkos.forall.detach.split
  br label %kokkos.forall.inc

kokkos.forall.body.entry:                         ; No predecessors!
  %this.addr.i104 = alloca %"class.Kokkos::View"*, align 8
  %i0.addr.i105 = alloca i32*, align 8
  %i1.addr.i106 = alloca i32*, align 8
  %this.addr.i65 = alloca %"class.Kokkos::View"*, align 8
  %i0.addr.i66 = alloca i32*, align 8
  %i1.addr.i67 = alloca i32*, align 8
  %ref.tmp5 = alloca i32, align 4
  %exn.slot7 = alloca i8*
  %ehselector.slot8 = alloca i32
  %ref.tmp13 = alloca i32, align 4
  br label %kokkos.forall.pfor.body

kokkos.forall.pfor.body:                          ; preds = %kokkos.forall.body.entry
  %41 = load i32, i32* %i4, align 4
  %conv = sitofp i32 %41 to double
  %mul = fmul double %conv, 1.000000e+02
  store i32 0, i32* %ref.tmp5, align 4
  store %"class.Kokkos::View"* %a, %"class.Kokkos::View"** %this.addr.i65, align 8
  store i32* %i4, i32** %i0.addr.i66, align 8
  store i32* %ref.tmp5, i32** %i1.addr.i67, align 8
  %this1.i68 = load %"class.Kokkos::View"*, %"class.Kokkos::View"** %this.addr.i65, align 8
  %m_map.i69 = getelementptr inbounds %"class.Kokkos::View", %"class.Kokkos::View"* %this1.i68, i32 0, i32 1
  %m_handle.i70 = getelementptr inbounds %"class.Kokkos::Impl::ViewMapping", %"class.Kokkos::Impl::ViewMapping"* %m_map.i69, i32 0, i32 0
  %42 = load double*, double** %m_handle.i70, align 8
  %43 = load i32*, i32** %i1.addr.i67, align 8
  %44 = load i32, i32* %43, align 4
  %conv.i71 = sext i32 %44 to i64
  %m_map2.i72 = getelementptr inbounds %"class.Kokkos::View", %"class.Kokkos::View"* %this1.i68, i32 0, i32 1
  %m_offset.i73 = getelementptr inbounds %"class.Kokkos::Impl::ViewMapping", %"class.Kokkos::Impl::ViewMapping"* %m_map2.i72, i32 0, i32 1
  %m_stride.i74 = getelementptr inbounds %"struct.Kokkos::Impl::ViewOffset", %"struct.Kokkos::Impl::ViewOffset"* %m_offset.i73, i32 0, i32 1
  %45 = load i64, i64* %m_stride.i74, align 8
  %46 = load i32*, i32** %i0.addr.i66, align 8
  %47 = load i32, i32* %46, align 4
  %conv3.i = sext i32 %47 to i64
  %mul.i75 = mul i64 %45, %conv3.i
  %add.i76 = add i64 %conv.i71, %mul.i75
  %arrayidx.i77 = getelementptr inbounds double, double* %42, i64 %add.i76
  br label %invoke.cont9

invoke.cont9:                                     ; preds = %kokkos.forall.pfor.body
  %48 = load double, double* %arrayidx.i77, align 8
  %add = fadd double %48, %mul
  store double %add, double* %arrayidx.i77, align 8
  %49 = load i32, i32* %i4, align 4
  %conv11 = sitofp i32 %49 to double
  %mul12 = fmul double %conv11, 1.000000e+04
  store i32 1, i32* %ref.tmp13, align 4
  store %"class.Kokkos::View"* %a, %"class.Kokkos::View"** %this.addr.i104, align 8
  store i32* %i4, i32** %i0.addr.i105, align 8
  store i32* %ref.tmp13, i32** %i1.addr.i106, align 8
  %this1.i107 = load %"class.Kokkos::View"*, %"class.Kokkos::View"** %this.addr.i104, align 8
  %m_map.i108 = getelementptr inbounds %"class.Kokkos::View", %"class.Kokkos::View"* %this1.i107, i32 0, i32 1
  %m_handle.i109 = getelementptr inbounds %"class.Kokkos::Impl::ViewMapping", %"class.Kokkos::Impl::ViewMapping"* %m_map.i108, i32 0, i32 0
  %50 = load double*, double** %m_handle.i109, align 8
  %51 = load i32*, i32** %i1.addr.i106, align 8
  %52 = load i32, i32* %51, align 4
  %conv.i110 = sext i32 %52 to i64
  %m_map2.i111 = getelementptr inbounds %"class.Kokkos::View", %"class.Kokkos::View"* %this1.i107, i32 0, i32 1
  %m_offset.i112 = getelementptr inbounds %"class.Kokkos::Impl::ViewMapping", %"class.Kokkos::Impl::ViewMapping"* %m_map2.i111, i32 0, i32 1
  %m_stride.i113 = getelementptr inbounds %"struct.Kokkos::Impl::ViewOffset", %"struct.Kokkos::Impl::ViewOffset"* %m_offset.i112, i32 0, i32 1
  %53 = load i64, i64* %m_stride.i113, align 8
  %54 = load i32*, i32** %i0.addr.i105, align 8
  %55 = load i32, i32* %54, align 4
  %conv3.i114 = sext i32 %55 to i64
  %mul.i115 = mul i64 %53, %conv3.i114
  %add.i116 = add i64 %conv.i110, %mul.i115
  %arrayidx.i117 = getelementptr inbounds double, double* %50, i64 %add.i116
  br label %invoke.cont14

invoke.cont14:                                    ; preds = %invoke.cont9
  %56 = load double, double* %arrayidx.i117, align 8
  %add16 = fadd double %56, %mul12
  store double %add16, double* %arrayidx.i117, align 8
  br label %kokkos.forall.preattach

kokkos.forall.preattach:                          ; preds = %invoke.cont14
  unreachable

kokkos.forall.inc:                                ; preds = %kokkos.forall.detach.split.split
  %57 = load i32, i32* %i4, align 4
  %58 = add i32 %57, 1
  store i32 %58, i32* %i4, align 4
  br label %kokkos.forall.cond, !llvm.loop !2

lpad6:                                            ; No predecessors!
  %59 = landingpad { i8*, i32 }
          cleanup
  %60 = extractvalue { i8*, i32 } %59, 0
  store i8* %60, i8** %exn.slot7, align 8
  %61 = extractvalue { i8*, i32 } %59, 1
  store i32 %61, i32* %ehselector.slot8, align 4
  br label %det.rethrow

det.rethrow:                                      ; preds = %lpad6
  br label %ehcleanup

kokkos.forall.end:                                ; preds = %kokkos.forall.cond
  %62 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 0
  %63 = load volatile i32, i32* %62
  %64 = and i32 %63, 2
  %65 = icmp eq i32 %64, 0
  br i1 %65, label %__cilk_sync.exit, label %cilk.sync.savestate.i

cilk.sync.savestate.i:                            ; preds = %kokkos.forall.end
  %66 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 9
  %67 = load { %struct.__cilkrts_pedigree }, { %struct.__cilkrts_pedigree }* %66
  %68 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 3
  %69 = load volatile %struct.__cilkrts_worker*, %struct.__cilkrts_worker** %68
  %70 = getelementptr inbounds %struct.__cilkrts_worker, %struct.__cilkrts_worker* %69, i32 0, i32 12
  %71 = load %struct.__cilkrts_pedigree, %struct.__cilkrts_pedigree* %70
  %72 = insertvalue { %struct.__cilkrts_pedigree } %67, %struct.__cilkrts_pedigree %71, 0
  %73 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 6
  %74 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 7
  call void asm sideeffect "stmxcsr $0\0A\09fnstcw $1", "*m,*m,~{dirflag},~{fpsr},~{flags}"(i32* %73, i16* %74)
  %75 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 5
  %76 = call i8* @llvm.frameaddress(i32 0)
  %77 = getelementptr inbounds [5 x i8*], [5 x i8*]* %75, i32 0, i32 0
  store volatile i8* %76, i8** %77
  %78 = call i8* @llvm.stacksave()
  %79 = getelementptr inbounds [5 x i8*], [5 x i8*]* %75, i32 0, i32 2
  store volatile i8* %78, i8** %79
  %80 = bitcast [5 x i8*]* %75 to i8*
  %81 = call i32 @llvm.eh.sjlj.setjmp(i8* %80) #12
  %82 = icmp eq i32 %81, 0
  br i1 %82, label %cilk.sync.runtimecall.i, label %cilk.sync.excepting.i

cilk.sync.runtimecall.i:                          ; preds = %cilk.sync.savestate.i
  call void @__cilkrts_sync(%struct.__cilkrts_stack_frame* %__cilkrts_sf)
  br label %__cilk_sync.exit

cilk.sync.excepting.i:                            ; preds = %cilk.sync.savestate.i
  %83 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 0
  %84 = load volatile i32, i32* %83
  %85 = and i32 %84, 16
  %86 = icmp eq i32 %85, 0
  br i1 %86, label %__cilk_sync.exit, label %cilk.sync.rethrow.i

cilk.sync.rethrow.i:                              ; preds = %cilk.sync.excepting.i
  call void @__cilkrts_rethrow(%struct.__cilkrts_stack_frame* %__cilkrts_sf) #13
  unreachable

__cilk_sync.exit:                                 ; preds = %kokkos.forall.end, %cilk.sync.runtimecall.i, %cilk.sync.excepting.i
  %87 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 3
  %88 = load volatile %struct.__cilkrts_worker*, %struct.__cilkrts_worker** %87
  %89 = getelementptr inbounds %struct.__cilkrts_worker, %struct.__cilkrts_worker* %88, i32 0, i32 12
  %90 = getelementptr inbounds %struct.__cilkrts_pedigree, %struct.__cilkrts_pedigree* %89, i32 0, i32 0
  %91 = load i64, i64* %90
  %92 = add i64 %91, 1
  store i64 %92, i64* %90
  br label %kokkos.end.continue

kokkos.end.continue:                              ; preds = %__cilk_sync.exit
  store i64 0, i64* %i17, align 8
  br label %for.cond18

for.cond18:                                       ; preds = %for.inc47, %kokkos.end.continue
  %93 = load i64, i64* %i17, align 8
  %cmp19 = icmp ult i64 %93, 1024
  br i1 %cmp19, label %for.body20, label %for.end49

for.body20:                                       ; preds = %for.cond18
  store i32 0, i32* %ref.tmp21, align 4
  store %"class.Kokkos::View"* %a, %"class.Kokkos::View"** %this.addr.i91, align 8
  store i64* %i17, i64** %i0.addr.i92, align 8
  store i32* %ref.tmp21, i32** %i1.addr.i93, align 8
  %this1.i94 = load %"class.Kokkos::View"*, %"class.Kokkos::View"** %this.addr.i91, align 8
  %m_map.i95 = getelementptr inbounds %"class.Kokkos::View", %"class.Kokkos::View"* %this1.i94, i32 0, i32 1
  %m_handle.i96 = getelementptr inbounds %"class.Kokkos::Impl::ViewMapping", %"class.Kokkos::Impl::ViewMapping"* %m_map.i95, i32 0, i32 0
  %94 = load double*, double** %m_handle.i96, align 8
  %95 = load i32*, i32** %i1.addr.i93, align 8
  %96 = load i32, i32* %95, align 4
  %conv.i97 = sext i32 %96 to i64
  %m_map2.i98 = getelementptr inbounds %"class.Kokkos::View", %"class.Kokkos::View"* %this1.i94, i32 0, i32 1
  %m_offset.i99 = getelementptr inbounds %"class.Kokkos::Impl::ViewMapping", %"class.Kokkos::Impl::ViewMapping"* %m_map2.i98, i32 0, i32 1
  %m_stride.i100 = getelementptr inbounds %"struct.Kokkos::Impl::ViewOffset", %"struct.Kokkos::Impl::ViewOffset"* %m_offset.i99, i32 0, i32 1
  %97 = load i64, i64* %m_stride.i100, align 8
  %98 = load i64*, i64** %i0.addr.i92, align 8
  %99 = load i64, i64* %98, align 8
  %mul.i101 = mul i64 %97, %99
  %add.i102 = add i64 %conv.i97, %mul.i101
  %arrayidx.i103 = getelementptr inbounds double, double* %94, i64 %add.i102
  br label %invoke.cont22

invoke.cont22:                                    ; preds = %for.body20
  %100 = load double, double* %arrayidx.i103, align 8
  store double %100, double* %ai0, align 8
  store i32 1, i32* %ref.tmp24, align 4
  store %"class.Kokkos::View"* %a, %"class.Kokkos::View"** %this.addr.i78, align 8
  store i64* %i17, i64** %i0.addr.i79, align 8
  store i32* %ref.tmp24, i32** %i1.addr.i80, align 8
  %this1.i81 = load %"class.Kokkos::View"*, %"class.Kokkos::View"** %this.addr.i78, align 8
  %m_map.i82 = getelementptr inbounds %"class.Kokkos::View", %"class.Kokkos::View"* %this1.i81, i32 0, i32 1
  %m_handle.i83 = getelementptr inbounds %"class.Kokkos::Impl::ViewMapping", %"class.Kokkos::Impl::ViewMapping"* %m_map.i82, i32 0, i32 0
  %101 = load double*, double** %m_handle.i83, align 8
  %102 = load i32*, i32** %i1.addr.i80, align 8
  %103 = load i32, i32* %102, align 4
  %conv.i84 = sext i32 %103 to i64
  %m_map2.i85 = getelementptr inbounds %"class.Kokkos::View", %"class.Kokkos::View"* %this1.i81, i32 0, i32 1
  %m_offset.i86 = getelementptr inbounds %"class.Kokkos::Impl::ViewMapping", %"class.Kokkos::Impl::ViewMapping"* %m_map2.i85, i32 0, i32 1
  %m_stride.i87 = getelementptr inbounds %"struct.Kokkos::Impl::ViewOffset", %"struct.Kokkos::Impl::ViewOffset"* %m_offset.i86, i32 0, i32 1
  %104 = load i64, i64* %m_stride.i87, align 8
  %105 = load i64*, i64** %i0.addr.i79, align 8
  %106 = load i64, i64* %105, align 8
  %mul.i88 = mul i64 %104, %106
  %add.i89 = add i64 %conv.i84, %mul.i88
  %arrayidx.i90 = getelementptr inbounds double, double* %101, i64 %add.i89
  br label %invoke.cont25

invoke.cont25:                                    ; preds = %invoke.cont22
  %107 = load double, double* %arrayidx.i90, align 8
  store double %107, double* %ai1, align 8
  %call28 = invoke dereferenceable(272) %"class.std::basic_ostream"* @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(%"class.std::basic_ostream"* dereferenceable(272) @_ZSt4cout, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.1, i32 0, i32 0))
          to label %invoke.cont27 unwind label %lpad

invoke.cont27:                                    ; preds = %invoke.cont25
  %108 = load i64, i64* %i17, align 8
  %call30 = invoke dereferenceable(272) %"class.std::basic_ostream"* @_ZNSolsEm(%"class.std::basic_ostream"* %call28, i64 %108)
          to label %invoke.cont29 unwind label %lpad

invoke.cont29:                                    ; preds = %invoke.cont27
  %call32 = invoke dereferenceable(272) %"class.std::basic_ostream"* @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(%"class.std::basic_ostream"* dereferenceable(272) %call30, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @.str.2, i32 0, i32 0))
          to label %invoke.cont31 unwind label %lpad

invoke.cont31:                                    ; preds = %invoke.cont29
  %109 = load double, double* %ai0, align 8
  %call34 = invoke dereferenceable(272) %"class.std::basic_ostream"* @_ZNSolsEd(%"class.std::basic_ostream"* %call32, double %109)
          to label %invoke.cont33 unwind label %lpad

invoke.cont33:                                    ; preds = %invoke.cont31
  %call36 = invoke dereferenceable(272) %"class.std::basic_ostream"* @_ZNSolsEPFRSoS_E(%"class.std::basic_ostream"* %call34, %"class.std::basic_ostream"* (%"class.std::basic_ostream"*)* @_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_)
          to label %invoke.cont35 unwind label %lpad

invoke.cont35:                                    ; preds = %invoke.cont33
  %call38 = invoke dereferenceable(272) %"class.std::basic_ostream"* @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(%"class.std::basic_ostream"* dereferenceable(272) @_ZSt4cout, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.1, i32 0, i32 0))
          to label %invoke.cont37 unwind label %lpad

invoke.cont37:                                    ; preds = %invoke.cont35
  %110 = load i64, i64* %i17, align 8
  %call40 = invoke dereferenceable(272) %"class.std::basic_ostream"* @_ZNSolsEm(%"class.std::basic_ostream"* %call38, i64 %110)
          to label %invoke.cont39 unwind label %lpad

invoke.cont39:                                    ; preds = %invoke.cont37
  %call42 = invoke dereferenceable(272) %"class.std::basic_ostream"* @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(%"class.std::basic_ostream"* dereferenceable(272) %call40, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @.str.3, i32 0, i32 0))
          to label %invoke.cont41 unwind label %lpad

invoke.cont41:                                    ; preds = %invoke.cont39
  %111 = load double, double* %ai1, align 8
  %call44 = invoke dereferenceable(272) %"class.std::basic_ostream"* @_ZNSolsEd(%"class.std::basic_ostream"* %call42, double %111)
          to label %invoke.cont43 unwind label %lpad

invoke.cont43:                                    ; preds = %invoke.cont41
  %call46 = invoke dereferenceable(272) %"class.std::basic_ostream"* @_ZNSolsEPFRSoS_E(%"class.std::basic_ostream"* %call44, %"class.std::basic_ostream"* (%"class.std::basic_ostream"*)* @_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_)
          to label %invoke.cont45 unwind label %lpad

invoke.cont45:                                    ; preds = %invoke.cont43
  br label %for.inc47

for.inc47:                                        ; preds = %invoke.cont45
  %112 = load i64, i64* %i17, align 8
  %inc48 = add i64 %112, 1
  store i64 %inc48, i64* %i17, align 8
  br label %for.cond18

for.end49:                                        ; preds = %for.cond18
  invoke void @_ZN6Kokkos8finalizeEv()
          to label %invoke.cont50 unwind label %lpad

invoke.cont50:                                    ; preds = %for.end49
  store i32 0, i32* %retval, align 4
  call void @_ZN6Kokkos4ViewIA2_PdJEED2Ev(%"class.Kokkos::View"* %a) #7
  %113 = load i32, i32* %retval, align 4
  %114 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 2
  %115 = load volatile %struct.__cilkrts_stack_frame*, %struct.__cilkrts_stack_frame** %114
  %116 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 3
  %117 = load volatile %struct.__cilkrts_worker*, %struct.__cilkrts_worker** %116
  %118 = getelementptr inbounds %struct.__cilkrts_worker, %struct.__cilkrts_worker* %117, i32 0, i32 9
  store volatile %struct.__cilkrts_stack_frame* %115, %struct.__cilkrts_stack_frame** %118
  %119 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 2
  store volatile %struct.__cilkrts_stack_frame* null, %struct.__cilkrts_stack_frame** %119
  %120 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 0
  %121 = load volatile i32, i32* %120
  %122 = icmp ne i32 %121, 16777216
  br i1 %122, label %body.i, label %__cilk_parent_epilogue.exit

body.i:                                           ; preds = %invoke.cont50
  call void @__cilkrts_leave_frame(%struct.__cilkrts_stack_frame* %__cilkrts_sf) #7
  br label %__cilk_parent_epilogue.exit

__cilk_parent_epilogue.exit:                      ; preds = %invoke.cont50, %body.i
  ret i32 %113

ehcleanup:                                        ; preds = %det.rethrow, %lpad
  call void @_ZN6Kokkos4ViewIA2_PdJEED2Ev(%"class.Kokkos::View"* %a) #7
  br label %eh.resume

eh.resume:                                        ; preds = %ehcleanup
  %exn = load i8*, i8** %exn.slot, align 8
  %sel = load i32, i32* %ehselector.slot, align 4
  %lpad.val = insertvalue { i8*, i32 } undef, i8* %exn, 0
  %lpad.val51 = insertvalue { i8*, i32 } %lpad.val, i32 %sel, 1
  resume { i8*, i32 } %lpad.val51
}

declare void @_ZN6Kokkos10initializeERiPPc(i32* dereferenceable(4), i8**) #1

; Function Attrs: noinline optnone uwtable
define linkonce_odr void @_ZN6Kokkos4ViewIA2_PdJEEC2IA2_cEERKT_NSt9enable_ifIXsr6Kokkos4Impl13is_view_labelIS6_EE5valueEKmE4typeEmmmmmmm(%"class.Kokkos::View"* %this, [2 x i8]* dereferenceable(2) %arg_label, i64 %arg_N0, i64 %arg_N1, i64 %arg_N2, i64 %arg_N3, i64 %arg_N4, i64 %arg_N5, i64 %arg_N6, i64 %arg_N7) unnamed_addr #2 comdat align 2 personality i8* bitcast (i32 (...)* @__gxx_personality_v0 to i8*) {
entry:
  %this.addr = alloca %"class.Kokkos::View"*, align 8
  %arg_label.addr = alloca [2 x i8]*, align 8
  %arg_N0.addr = alloca i64, align 8
  %arg_N1.addr = alloca i64, align 8
  %arg_N2.addr = alloca i64, align 8
  %arg_N3.addr = alloca i64, align 8
  %arg_N4.addr = alloca i64, align 8
  %arg_N5.addr = alloca i64, align 8
  %arg_N6.addr = alloca i64, align 8
  %arg_N7.addr = alloca i64, align 8
  %ref.tmp = alloca %"struct.Kokkos::Impl::ViewCtorProp", align 8
  %ref.tmp2 = alloca %"struct.Kokkos::LayoutRight", align 8
  %exn.slot = alloca i8*
  %ehselector.slot = alloca i32
  %ref.tmp4 = alloca %"class.std::__cxx11::basic_string", align 8
  store %"class.Kokkos::View"* %this, %"class.Kokkos::View"** %this.addr, align 8
  store [2 x i8]* %arg_label, [2 x i8]** %arg_label.addr, align 8
  store i64 %arg_N0, i64* %arg_N0.addr, align 8
  store i64 %arg_N1, i64* %arg_N1.addr, align 8
  store i64 %arg_N2, i64* %arg_N2.addr, align 8
  store i64 %arg_N3, i64* %arg_N3.addr, align 8
  store i64 %arg_N4, i64* %arg_N4.addr, align 8
  store i64 %arg_N5, i64* %arg_N5.addr, align 8
  store i64 %arg_N6, i64* %arg_N6.addr, align 8
  store i64 %arg_N7, i64* %arg_N7.addr, align 8
  %this1 = load %"class.Kokkos::View"*, %"class.Kokkos::View"** %this.addr, align 8
  %0 = load [2 x i8]*, [2 x i8]** %arg_label.addr, align 8
  call void @_ZN6Kokkos4Impl12ViewCtorPropIJNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEEEC2IJA2_cEEEDpRKT_(%"struct.Kokkos::Impl::ViewCtorProp"* %ref.tmp, [2 x i8]* dereferenceable(2) %0)
  %1 = load i64, i64* %arg_N0.addr, align 8
  %2 = load i64, i64* %arg_N1.addr, align 8
  %3 = load i64, i64* %arg_N2.addr, align 8
  %4 = load i64, i64* %arg_N3.addr, align 8
  %5 = load i64, i64* %arg_N4.addr, align 8
  %6 = load i64, i64* %arg_N5.addr, align 8
  %7 = load i64, i64* %arg_N6.addr, align 8
  %8 = load i64, i64* %arg_N7.addr, align 8
  invoke void @_ZN6Kokkos11LayoutRightC2Emmmmmmmm(%"struct.Kokkos::LayoutRight"* %ref.tmp2, i64 %1, i64 %2, i64 %3, i64 %4, i64 %5, i64 %6, i64 %7, i64 %8)
          to label %invoke.cont unwind label %lpad

invoke.cont:                                      ; preds = %entry
  invoke void @_ZN6Kokkos4ViewIA2_PdJEEC2IJNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEEEERKNS_4Impl12ViewCtorPropIJDpT_EEERKNSt9enable_ifIXntsr4Impl12ViewCtorPropISE_EE11has_pointerENS_11LayoutRightEE4typeE(%"class.Kokkos::View"* %this1, %"struct.Kokkos::Impl::ViewCtorProp"* dereferenceable(32) %ref.tmp, %"struct.Kokkos::LayoutRight"* dereferenceable(64) %ref.tmp2)
          to label %invoke.cont3 unwind label %lpad

invoke.cont3:                                     ; preds = %invoke.cont
  call void @_ZN6Kokkos4Impl12ViewCtorPropIJNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEEED2Ev(%"struct.Kokkos::Impl::ViewCtorProp"* %ref.tmp) #7
  %9 = load i64, i64* %arg_N0.addr, align 8
  %10 = load i64, i64* %arg_N1.addr, align 8
  %11 = load i64, i64* %arg_N2.addr, align 8
  %12 = load i64, i64* %arg_N3.addr, align 8
  %13 = load i64, i64* %arg_N4.addr, align 8
  %14 = load i64, i64* %arg_N5.addr, align 8
  %15 = load i64, i64* %arg_N6.addr, align 8
  %16 = load i64, i64* %arg_N7.addr, align 8
  invoke void @_ZNK6Kokkos4ViewIA2_PdJEE5labelB5cxx11Ev(%"class.std::__cxx11::basic_string"* sret %ref.tmp4, %"class.Kokkos::View"* %this1)
          to label %invoke.cont6 unwind label %lpad5

invoke.cont6:                                     ; preds = %invoke.cont3
  invoke void @_ZN6Kokkos4Impl23runtime_check_rank_hostEmbmmmmmmmmRKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE(i64 1, i1 zeroext true, i64 %9, i64 %10, i64 %11, i64 %12, i64 %13, i64 %14, i64 %15, i64 %16, %"class.std::__cxx11::basic_string"* dereferenceable(32) %ref.tmp4)
          to label %invoke.cont8 unwind label %lpad7

invoke.cont8:                                     ; preds = %invoke.cont6
  call void @_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED1Ev(%"class.std::__cxx11::basic_string"* %ref.tmp4) #7
  ret void

lpad:                                             ; preds = %invoke.cont, %entry
  %17 = landingpad { i8*, i32 }
          cleanup
  %18 = extractvalue { i8*, i32 } %17, 0
  store i8* %18, i8** %exn.slot, align 8
  %19 = extractvalue { i8*, i32 } %17, 1
  store i32 %19, i32* %ehselector.slot, align 4
  call void @_ZN6Kokkos4Impl12ViewCtorPropIJNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEEED2Ev(%"struct.Kokkos::Impl::ViewCtorProp"* %ref.tmp) #7
  br label %eh.resume

lpad5:                                            ; preds = %invoke.cont3
  %20 = landingpad { i8*, i32 }
          cleanup
  %21 = extractvalue { i8*, i32 } %20, 0
  store i8* %21, i8** %exn.slot, align 8
  %22 = extractvalue { i8*, i32 } %20, 1
  store i32 %22, i32* %ehselector.slot, align 4
  br label %ehcleanup

lpad7:                                            ; preds = %invoke.cont6
  %23 = landingpad { i8*, i32 }
          cleanup
  %24 = extractvalue { i8*, i32 } %23, 0
  store i8* %24, i8** %exn.slot, align 8
  %25 = extractvalue { i8*, i32 } %23, 1
  store i32 %25, i32* %ehselector.slot, align 4
  call void @_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED1Ev(%"class.std::__cxx11::basic_string"* %ref.tmp4) #7
  br label %ehcleanup

ehcleanup:                                        ; preds = %lpad7, %lpad5
  call void @_ZN6Kokkos4ViewIA2_PdJEED2Ev(%"class.Kokkos::View"* %this1) #7
  br label %eh.resume

eh.resume:                                        ; preds = %ehcleanup, %lpad
  %exn = load i8*, i8** %exn.slot, align 8
  %sel = load i32, i32* %ehselector.slot, align 4
  %lpad.val = insertvalue { i8*, i32 } undef, i8* %exn, 0
  %lpad.val9 = insertvalue { i8*, i32 } %lpad.val, i32 %sel, 1
  resume { i8*, i32 } %lpad.val9
}

declare i32 @__gxx_personality_v0(...)

; Function Attrs: argmemonly nounwind
declare token @llvm.syncregion.start() #3

declare dereferenceable(272) %"class.std::basic_ostream"* @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(%"class.std::basic_ostream"* dereferenceable(272), i8*) #1

declare dereferenceable(272) %"class.std::basic_ostream"* @_ZNSolsEm(%"class.std::basic_ostream"*, i64) #1

declare dereferenceable(272) %"class.std::basic_ostream"* @_ZNSolsEd(%"class.std::basic_ostream"*, double) #1

declare dereferenceable(272) %"class.std::basic_ostream"* @_ZNSolsEPFRSoS_E(%"class.std::basic_ostream"*, %"class.std::basic_ostream"* (%"class.std::basic_ostream"*)*) #1

declare dereferenceable(272) %"class.std::basic_ostream"* @_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_(%"class.std::basic_ostream"* dereferenceable(272)) #1

declare void @_ZN6Kokkos8finalizeEv() #1

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr void @_ZN6Kokkos4ViewIA2_PdJEED2Ev(%"class.Kokkos::View"* %this) unnamed_addr #4 comdat align 2 personality i32 (...)* @__gxx_personality_v0 {
entry:
  %this.addr.i = alloca %"union.Kokkos::Impl::SharedAllocationTracker"*, align 8
  %this.addr = alloca %"class.Kokkos::View"*, align 8
  store %"class.Kokkos::View"* %this, %"class.Kokkos::View"** %this.addr, align 8
  %this1 = load %"class.Kokkos::View"*, %"class.Kokkos::View"** %this.addr, align 8
  %m_map = getelementptr inbounds %"class.Kokkos::View", %"class.Kokkos::View"* %this1, i32 0, i32 1
  call void @_ZN6Kokkos4Impl11ViewMappingINS_10ViewTraitsIA2_PdJEEEJvEED2Ev(%"class.Kokkos::Impl::ViewMapping"* %m_map) #7
  %m_track = getelementptr inbounds %"class.Kokkos::View", %"class.Kokkos::View"* %this1, i32 0, i32 0
  store %"union.Kokkos::Impl::SharedAllocationTracker"* %m_track, %"union.Kokkos::Impl::SharedAllocationTracker"** %this.addr.i, align 8
  %this1.i = load %"union.Kokkos::Impl::SharedAllocationTracker"*, %"union.Kokkos::Impl::SharedAllocationTracker"** %this.addr.i, align 8
  %m_record_bits.i = bitcast %"union.Kokkos::Impl::SharedAllocationTracker"* %this1.i to i64*
  %0 = load i64, i64* %m_record_bits.i, align 8
  %and.i = and i64 %0, 1
  %tobool.i = icmp ne i64 %and.i, 0
  br i1 %tobool.i, label %_ZN6Kokkos4Impl23SharedAllocationTrackerD2Ev.exit, label %if.then.i

if.then.i:                                        ; preds = %entry
  %m_record.i = bitcast %"union.Kokkos::Impl::SharedAllocationTracker"* %this1.i to %"class.Kokkos::Impl::SharedAllocationRecord"**
  %1 = load %"class.Kokkos::Impl::SharedAllocationRecord"*, %"class.Kokkos::Impl::SharedAllocationRecord"** %m_record.i, align 8
  %call.i = invoke %"class.Kokkos::Impl::SharedAllocationRecord"* @_ZN6Kokkos4Impl22SharedAllocationRecordIvvE9decrementEPS2_(%"class.Kokkos::Impl::SharedAllocationRecord"* %1)
          to label %invoke.cont.i unwind label %terminate.lpad.i

invoke.cont.i:                                    ; preds = %if.then.i
  br label %_ZN6Kokkos4Impl23SharedAllocationTrackerD2Ev.exit

terminate.lpad.i:                                 ; preds = %if.then.i
  %2 = landingpad { i8*, i32 }
          catch i8* null
  %3 = extractvalue { i8*, i32 } %2, 0
  call void @__clang_call_terminate(i8* %3) #14
  unreachable

_ZN6Kokkos4Impl23SharedAllocationTrackerD2Ev.exit: ; preds = %entry, %invoke.cont.i
  ret void
}

; Function Attrs: noinline uwtable
define internal void @__cxx_global_var_init() #5 section ".text.startup" {
entry:
  call void @_ZNSt8ios_base4InitC1Ev(%"class.std::ios_base::Init"* @_ZStL8__ioinit)
  %0 = call i32 @__cxa_atexit(void (i8*)* bitcast (void (%"class.std::ios_base::Init"*)* @_ZNSt8ios_base4InitD1Ev to void (i8*)*), i8* getelementptr inbounds (%"class.std::ios_base::Init", %"class.std::ios_base::Init"* @_ZStL8__ioinit, i32 0, i32 0), i8* @__dso_handle) #7
  ret void
}

declare void @_ZNSt8ios_base4InitC1Ev(%"class.std::ios_base::Init"*) unnamed_addr #1

; Function Attrs: nounwind
declare void @_ZNSt8ios_base4InitD1Ev(%"class.std::ios_base::Init"*) unnamed_addr #6

; Function Attrs: nounwind
declare i32 @__cxa_atexit(void (i8*)*, i8*, i8*) #7

; Function Attrs: noinline optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl12ViewCtorPropIJNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEEEC2IJA2_cEEEDpRKT_(%"struct.Kokkos::Impl::ViewCtorProp"* %this, [2 x i8]* dereferenceable(2) %args) unnamed_addr #2 comdat align 2 personality i8* bitcast (i32 (...)* @__gxx_personality_v0 to i8*) {
entry:
  %this.addr = alloca %"struct.Kokkos::Impl::ViewCtorProp"*, align 8
  %args.addr = alloca [2 x i8]*, align 8
  %ref.tmp = alloca %"class.std::__cxx11::basic_string", align 8
  %ref.tmp2 = alloca %"class.std::allocator", align 1
  %exn.slot = alloca i8*
  %ehselector.slot = alloca i32
  store %"struct.Kokkos::Impl::ViewCtorProp"* %this, %"struct.Kokkos::Impl::ViewCtorProp"** %this.addr, align 8
  store [2 x i8]* %args, [2 x i8]** %args.addr, align 8
  %this1 = load %"struct.Kokkos::Impl::ViewCtorProp"*, %"struct.Kokkos::Impl::ViewCtorProp"** %this.addr, align 8
  %0 = bitcast %"struct.Kokkos::Impl::ViewCtorProp"* %this1 to %"struct.Kokkos::Impl::ViewCtorProp.0"*
  %1 = load [2 x i8]*, [2 x i8]** %args.addr, align 8
  %arraydecay = getelementptr inbounds [2 x i8], [2 x i8]* %1, i32 0, i32 0
  call void @_ZNSaIcEC1Ev(%"class.std::allocator"* %ref.tmp2) #7
  invoke void @_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EPKcRKS3_(%"class.std::__cxx11::basic_string"* %ref.tmp, i8* %arraydecay, %"class.std::allocator"* dereferenceable(1) %ref.tmp2)
          to label %invoke.cont unwind label %lpad

invoke.cont:                                      ; preds = %entry
  invoke void @_ZN6Kokkos4Impl12ViewCtorPropIJvNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEEEC2EOS7_(%"struct.Kokkos::Impl::ViewCtorProp.0"* %0, %"class.std::__cxx11::basic_string"* dereferenceable(32) %ref.tmp)
          to label %invoke.cont4 unwind label %lpad3

invoke.cont4:                                     ; preds = %invoke.cont
  call void @_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED1Ev(%"class.std::__cxx11::basic_string"* %ref.tmp) #7
  call void @_ZNSaIcED1Ev(%"class.std::allocator"* %ref.tmp2) #7
  ret void

lpad:                                             ; preds = %entry
  %2 = landingpad { i8*, i32 }
          cleanup
  %3 = extractvalue { i8*, i32 } %2, 0
  store i8* %3, i8** %exn.slot, align 8
  %4 = extractvalue { i8*, i32 } %2, 1
  store i32 %4, i32* %ehselector.slot, align 4
  br label %ehcleanup

lpad3:                                            ; preds = %invoke.cont
  %5 = landingpad { i8*, i32 }
          cleanup
  %6 = extractvalue { i8*, i32 } %5, 0
  store i8* %6, i8** %exn.slot, align 8
  %7 = extractvalue { i8*, i32 } %5, 1
  store i32 %7, i32* %ehselector.slot, align 4
  call void @_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED1Ev(%"class.std::__cxx11::basic_string"* %ref.tmp) #7
  br label %ehcleanup

ehcleanup:                                        ; preds = %lpad3, %lpad
  call void @_ZNSaIcED1Ev(%"class.std::allocator"* %ref.tmp2) #7
  br label %eh.resume

eh.resume:                                        ; preds = %ehcleanup
  %exn = load i8*, i8** %exn.slot, align 8
  %sel = load i32, i32* %ehselector.slot, align 4
  %lpad.val = insertvalue { i8*, i32 } undef, i8* %exn, 0
  %lpad.val5 = insertvalue { i8*, i32 } %lpad.val, i32 %sel, 1
  resume { i8*, i32 } %lpad.val5
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr void @_ZN6Kokkos11LayoutRightC2Emmmmmmmm(%"struct.Kokkos::LayoutRight"* %this, i64 %N0, i64 %N1, i64 %N2, i64 %N3, i64 %N4, i64 %N5, i64 %N6, i64 %N7) unnamed_addr #4 comdat align 2 {
entry:
  %this.addr = alloca %"struct.Kokkos::LayoutRight"*, align 8
  %N0.addr = alloca i64, align 8
  %N1.addr = alloca i64, align 8
  %N2.addr = alloca i64, align 8
  %N3.addr = alloca i64, align 8
  %N4.addr = alloca i64, align 8
  %N5.addr = alloca i64, align 8
  %N6.addr = alloca i64, align 8
  %N7.addr = alloca i64, align 8
  store %"struct.Kokkos::LayoutRight"* %this, %"struct.Kokkos::LayoutRight"** %this.addr, align 8
  store i64 %N0, i64* %N0.addr, align 8
  store i64 %N1, i64* %N1.addr, align 8
  store i64 %N2, i64* %N2.addr, align 8
  store i64 %N3, i64* %N3.addr, align 8
  store i64 %N4, i64* %N4.addr, align 8
  store i64 %N5, i64* %N5.addr, align 8
  store i64 %N6, i64* %N6.addr, align 8
  store i64 %N7, i64* %N7.addr, align 8
  %this1 = load %"struct.Kokkos::LayoutRight"*, %"struct.Kokkos::LayoutRight"** %this.addr, align 8
  %dimension = getelementptr inbounds %"struct.Kokkos::LayoutRight", %"struct.Kokkos::LayoutRight"* %this1, i32 0, i32 0
  %arrayinit.begin = getelementptr inbounds [8 x i64], [8 x i64]* %dimension, i64 0, i64 0
  %0 = load i64, i64* %N0.addr, align 8
  store i64 %0, i64* %arrayinit.begin, align 8
  %arrayinit.element = getelementptr inbounds i64, i64* %arrayinit.begin, i64 1
  %1 = load i64, i64* %N1.addr, align 8
  store i64 %1, i64* %arrayinit.element, align 8
  %arrayinit.element2 = getelementptr inbounds i64, i64* %arrayinit.element, i64 1
  %2 = load i64, i64* %N2.addr, align 8
  store i64 %2, i64* %arrayinit.element2, align 8
  %arrayinit.element3 = getelementptr inbounds i64, i64* %arrayinit.element2, i64 1
  %3 = load i64, i64* %N3.addr, align 8
  store i64 %3, i64* %arrayinit.element3, align 8
  %arrayinit.element4 = getelementptr inbounds i64, i64* %arrayinit.element3, i64 1
  %4 = load i64, i64* %N4.addr, align 8
  store i64 %4, i64* %arrayinit.element4, align 8
  %arrayinit.element5 = getelementptr inbounds i64, i64* %arrayinit.element4, i64 1
  %5 = load i64, i64* %N5.addr, align 8
  store i64 %5, i64* %arrayinit.element5, align 8
  %arrayinit.element6 = getelementptr inbounds i64, i64* %arrayinit.element5, i64 1
  %6 = load i64, i64* %N6.addr, align 8
  store i64 %6, i64* %arrayinit.element6, align 8
  %arrayinit.element7 = getelementptr inbounds i64, i64* %arrayinit.element6, i64 1
  %7 = load i64, i64* %N7.addr, align 8
  store i64 %7, i64* %arrayinit.element7, align 8
  ret void
}

; Function Attrs: noinline optnone uwtable
define linkonce_odr void @_ZN6Kokkos4ViewIA2_PdJEEC2IJNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEEEERKNS_4Impl12ViewCtorPropIJDpT_EEERKNSt9enable_ifIXntsr4Impl12ViewCtorPropISE_EE11has_pointerENS_11LayoutRightEE4typeE(%"class.Kokkos::View"* %this, %"struct.Kokkos::Impl::ViewCtorProp"* dereferenceable(32) %arg_prop, %"struct.Kokkos::LayoutRight"* dereferenceable(64) %arg_layout) unnamed_addr #2 comdat align 2 personality i8* bitcast (i32 (...)* @__gxx_personality_v0 to i8*) {
entry:
  %this.addr.i19 = alloca %"union.Kokkos::Impl::SharedAllocationTracker"*, align 8
  %this.addr.i = alloca %"union.Kokkos::Impl::SharedAllocationTracker"*, align 8
  %this.addr = alloca %"class.Kokkos::View"*, align 8
  %arg_prop.addr = alloca %"struct.Kokkos::Impl::ViewCtorProp"*, align 8
  %arg_layout.addr = alloca %"struct.Kokkos::LayoutRight"*, align 8
  %exn.slot = alloca i8*
  %ehselector.slot = alloca i32
  %ref.tmp = alloca %"class.std::__cxx11::basic_string", align 8
  %ref.tmp2 = alloca %"class.std::allocator", align 1
  %prop = alloca %"struct.Kokkos::Impl::ViewCtorProp.1", align 8
  %record = alloca %"class.Kokkos::Impl::SharedAllocationRecord"*, align 8
  store %"class.Kokkos::View"* %this, %"class.Kokkos::View"** %this.addr, align 8
  store %"struct.Kokkos::Impl::ViewCtorProp"* %arg_prop, %"struct.Kokkos::Impl::ViewCtorProp"** %arg_prop.addr, align 8
  store %"struct.Kokkos::LayoutRight"* %arg_layout, %"struct.Kokkos::LayoutRight"** %arg_layout.addr, align 8
  %this1 = load %"class.Kokkos::View"*, %"class.Kokkos::View"** %this.addr, align 8
  %0 = bitcast %"class.Kokkos::View"* %this1 to %"struct.Kokkos::ViewTraits"*
  %m_track = getelementptr inbounds %"class.Kokkos::View", %"class.Kokkos::View"* %this1, i32 0, i32 0
  store %"union.Kokkos::Impl::SharedAllocationTracker"* %m_track, %"union.Kokkos::Impl::SharedAllocationTracker"** %this.addr.i, align 8
  %this1.i = load %"union.Kokkos::Impl::SharedAllocationTracker"*, %"union.Kokkos::Impl::SharedAllocationTracker"** %this.addr.i, align 8
  %m_record_bits.i = bitcast %"union.Kokkos::Impl::SharedAllocationTracker"* %this1.i to i64*
  store i64 1, i64* %m_record_bits.i, align 8
  %m_map = getelementptr inbounds %"class.Kokkos::View", %"class.Kokkos::View"* %this1, i32 0, i32 1
  invoke void @_ZN6Kokkos4Impl11ViewMappingINS_10ViewTraitsIA2_PdJEEEJvEEC2Ev(%"class.Kokkos::Impl::ViewMapping"* %m_map)
          to label %invoke.cont unwind label %lpad

invoke.cont:                                      ; preds = %entry
  %call = call zeroext i1 @_ZN6Kokkos6OpenMP14is_initializedEv() #7
  br i1 %call, label %if.end, label %if.then

if.then:                                          ; preds = %invoke.cont
  call void @_ZNSaIcEC1Ev(%"class.std::allocator"* %ref.tmp2) #7
  invoke void @_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EPKcRKS3_(%"class.std::__cxx11::basic_string"* %ref.tmp, i8* getelementptr inbounds ([75 x i8], [75 x i8]* @.str.4, i32 0, i32 0), %"class.std::allocator"* dereferenceable(1) %ref.tmp2)
          to label %invoke.cont4 unwind label %lpad3

invoke.cont4:                                     ; preds = %if.then
  invoke void @_ZN6Kokkos4Impl23throw_runtime_exceptionERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE(%"class.std::__cxx11::basic_string"* dereferenceable(32) %ref.tmp)
          to label %invoke.cont6 unwind label %lpad5

invoke.cont6:                                     ; preds = %invoke.cont4
  call void @_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED1Ev(%"class.std::__cxx11::basic_string"* %ref.tmp) #7
  call void @_ZNSaIcED1Ev(%"class.std::allocator"* %ref.tmp2) #7
  br label %if.end

lpad:                                             ; preds = %entry
  %1 = landingpad { i8*, i32 }
          cleanup
  %2 = extractvalue { i8*, i32 } %1, 0
  store i8* %2, i8** %exn.slot, align 8
  %3 = extractvalue { i8*, i32 } %1, 1
  store i32 %3, i32* %ehselector.slot, align 4
  br label %ehcleanup17

lpad3:                                            ; preds = %if.then
  %4 = landingpad { i8*, i32 }
          cleanup
  %5 = extractvalue { i8*, i32 } %4, 0
  store i8* %5, i8** %exn.slot, align 8
  %6 = extractvalue { i8*, i32 } %4, 1
  store i32 %6, i32* %ehselector.slot, align 4
  br label %ehcleanup

lpad5:                                            ; preds = %invoke.cont4
  %7 = landingpad { i8*, i32 }
          cleanup
  %8 = extractvalue { i8*, i32 } %7, 0
  store i8* %8, i8** %exn.slot, align 8
  %9 = extractvalue { i8*, i32 } %7, 1
  store i32 %9, i32* %ehselector.slot, align 4
  call void @_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED1Ev(%"class.std::__cxx11::basic_string"* %ref.tmp) #7
  br label %ehcleanup

ehcleanup:                                        ; preds = %lpad5, %lpad3
  call void @_ZNSaIcED1Ev(%"class.std::allocator"* %ref.tmp2) #7
  br label %ehcleanup16

if.end:                                           ; preds = %invoke.cont6, %invoke.cont
  %10 = load %"struct.Kokkos::Impl::ViewCtorProp"*, %"struct.Kokkos::Impl::ViewCtorProp"** %arg_prop.addr, align 8
  invoke void @_ZN6Kokkos4Impl12ViewCtorPropIJNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEESt17integral_constantIjLj0EENS_9HostSpaceENS_6OpenMPEEEC2IJS7_EEERKNS1_IJDpT_EEE(%"struct.Kokkos::Impl::ViewCtorProp.1"* %prop, %"struct.Kokkos::Impl::ViewCtorProp"* dereferenceable(32) %10)
          to label %invoke.cont8 unwind label %lpad7

invoke.cont8:                                     ; preds = %if.end
  %m_map9 = getelementptr inbounds %"class.Kokkos::View", %"class.Kokkos::View"* %this1, i32 0, i32 1
  %11 = load %"struct.Kokkos::LayoutRight"*, %"struct.Kokkos::LayoutRight"** %arg_layout.addr, align 8
  %call12 = invoke %"class.Kokkos::Impl::SharedAllocationRecord"* @_ZN6Kokkos4Impl11ViewMappingINS_10ViewTraitsIA2_PdJEEEJvEE15allocate_sharedIJNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEESt17integral_constantIjLj0EENS_9HostSpaceENS_6OpenMPEEEEPNS0_22SharedAllocationRecordIvvEERKNS0_12ViewCtorPropIJDpT_EEERKNS_11LayoutRightE(%"class.Kokkos::Impl::ViewMapping"* %m_map9, %"struct.Kokkos::Impl::ViewCtorProp.1"* dereferenceable(40) %prop, %"struct.Kokkos::LayoutRight"* dereferenceable(64) %11)
          to label %invoke.cont11 unwind label %lpad10

invoke.cont11:                                    ; preds = %invoke.cont8
  store %"class.Kokkos::Impl::SharedAllocationRecord"* %call12, %"class.Kokkos::Impl::SharedAllocationRecord"** %record, align 8
  %m_track13 = getelementptr inbounds %"class.Kokkos::View", %"class.Kokkos::View"* %this1, i32 0, i32 0
  %12 = load %"class.Kokkos::Impl::SharedAllocationRecord"*, %"class.Kokkos::Impl::SharedAllocationRecord"** %record, align 8
  invoke void @_ZN6Kokkos4Impl23SharedAllocationTracker40assign_allocated_record_to_uninitializedEPNS0_22SharedAllocationRecordIvvEE(%"union.Kokkos::Impl::SharedAllocationTracker"* %m_track13, %"class.Kokkos::Impl::SharedAllocationRecord"* %12)
          to label %invoke.cont14 unwind label %lpad10

invoke.cont14:                                    ; preds = %invoke.cont11
  call void @_ZN6Kokkos4Impl12ViewCtorPropIJNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEESt17integral_constantIjLj0EENS_9HostSpaceENS_6OpenMPEEED2Ev(%"struct.Kokkos::Impl::ViewCtorProp.1"* %prop) #7
  ret void

lpad7:                                            ; preds = %if.end
  %13 = landingpad { i8*, i32 }
          cleanup
  %14 = extractvalue { i8*, i32 } %13, 0
  store i8* %14, i8** %exn.slot, align 8
  %15 = extractvalue { i8*, i32 } %13, 1
  store i32 %15, i32* %ehselector.slot, align 4
  br label %ehcleanup16

lpad10:                                           ; preds = %invoke.cont11, %invoke.cont8
  %16 = landingpad { i8*, i32 }
          cleanup
  %17 = extractvalue { i8*, i32 } %16, 0
  store i8* %17, i8** %exn.slot, align 8
  %18 = extractvalue { i8*, i32 } %16, 1
  store i32 %18, i32* %ehselector.slot, align 4
  call void @_ZN6Kokkos4Impl12ViewCtorPropIJNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEESt17integral_constantIjLj0EENS_9HostSpaceENS_6OpenMPEEED2Ev(%"struct.Kokkos::Impl::ViewCtorProp.1"* %prop) #7
  br label %ehcleanup16

ehcleanup16:                                      ; preds = %lpad10, %lpad7, %ehcleanup
  call void @_ZN6Kokkos4Impl11ViewMappingINS_10ViewTraitsIA2_PdJEEEJvEED2Ev(%"class.Kokkos::Impl::ViewMapping"* %m_map) #7
  br label %ehcleanup17

ehcleanup17:                                      ; preds = %ehcleanup16, %lpad
  store %"union.Kokkos::Impl::SharedAllocationTracker"* %m_track, %"union.Kokkos::Impl::SharedAllocationTracker"** %this.addr.i19, align 8
  %this1.i20 = load %"union.Kokkos::Impl::SharedAllocationTracker"*, %"union.Kokkos::Impl::SharedAllocationTracker"** %this.addr.i19, align 8
  %m_record_bits.i21 = bitcast %"union.Kokkos::Impl::SharedAllocationTracker"* %this1.i20 to i64*
  %19 = load i64, i64* %m_record_bits.i21, align 8
  %and.i = and i64 %19, 1
  %tobool.i = icmp ne i64 %and.i, 0
  br i1 %tobool.i, label %_ZN6Kokkos4Impl23SharedAllocationTrackerD2Ev.exit, label %if.then.i

if.then.i:                                        ; preds = %ehcleanup17
  %m_record.i = bitcast %"union.Kokkos::Impl::SharedAllocationTracker"* %this1.i20 to %"class.Kokkos::Impl::SharedAllocationRecord"**
  %20 = load %"class.Kokkos::Impl::SharedAllocationRecord"*, %"class.Kokkos::Impl::SharedAllocationRecord"** %m_record.i, align 8
  %call.i = invoke %"class.Kokkos::Impl::SharedAllocationRecord"* @_ZN6Kokkos4Impl22SharedAllocationRecordIvvE9decrementEPS2_(%"class.Kokkos::Impl::SharedAllocationRecord"* %20)
          to label %invoke.cont.i unwind label %terminate.lpad.i

invoke.cont.i:                                    ; preds = %if.then.i
  br label %_ZN6Kokkos4Impl23SharedAllocationTrackerD2Ev.exit

terminate.lpad.i:                                 ; preds = %if.then.i
  %21 = landingpad { i8*, i32 }
          catch i8* null
  %22 = extractvalue { i8*, i32 } %21, 0
  call void @__clang_call_terminate(i8* %22) #14
  unreachable

_ZN6Kokkos4Impl23SharedAllocationTrackerD2Ev.exit: ; preds = %ehcleanup17, %invoke.cont.i
  br label %eh.resume

eh.resume:                                        ; preds = %_ZN6Kokkos4Impl23SharedAllocationTrackerD2Ev.exit
  %exn = load i8*, i8** %exn.slot, align 8
  %sel = load i32, i32* %ehselector.slot, align 4
  %lpad.val = insertvalue { i8*, i32 } undef, i8* %exn, 0
  %lpad.val18 = insertvalue { i8*, i32 } %lpad.val, i32 %sel, 1
  resume { i8*, i32 } %lpad.val18
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl12ViewCtorPropIJNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEEED2Ev(%"struct.Kokkos::Impl::ViewCtorProp"* %this) unnamed_addr #4 comdat align 2 {
entry:
  %this.addr = alloca %"struct.Kokkos::Impl::ViewCtorProp"*, align 8
  store %"struct.Kokkos::Impl::ViewCtorProp"* %this, %"struct.Kokkos::Impl::ViewCtorProp"** %this.addr, align 8
  %this1 = load %"struct.Kokkos::Impl::ViewCtorProp"*, %"struct.Kokkos::Impl::ViewCtorProp"** %this.addr, align 8
  %0 = bitcast %"struct.Kokkos::Impl::ViewCtorProp"* %this1 to %"struct.Kokkos::Impl::ViewCtorProp.0"*
  call void @_ZN6Kokkos4Impl12ViewCtorPropIJvNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEEED2Ev(%"struct.Kokkos::Impl::ViewCtorProp.0"* %0) #7
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl23runtime_check_rank_hostEmbmmmmmmmmRKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE(i64 %dyn_rank, i1 zeroext %is_void_spec, i64 %i0, i64 %i1, i64 %i2, i64 %i3, i64 %i4, i64 %i5, i64 %i6, i64 %i7, %"class.std::__cxx11::basic_string"* dereferenceable(32) %label) #4 comdat {
entry:
  %dyn_rank.addr = alloca i64, align 8
  %is_void_spec.addr = alloca i8, align 1
  %i0.addr = alloca i64, align 8
  %i1.addr = alloca i64, align 8
  %i2.addr = alloca i64, align 8
  %i3.addr = alloca i64, align 8
  %i4.addr = alloca i64, align 8
  %i5.addr = alloca i64, align 8
  %i6.addr = alloca i64, align 8
  %i7.addr = alloca i64, align 8
  %label.addr = alloca %"class.std::__cxx11::basic_string"*, align 8
  store i64 %dyn_rank, i64* %dyn_rank.addr, align 8
  %frombool = zext i1 %is_void_spec to i8
  store i8 %frombool, i8* %is_void_spec.addr, align 1
  store i64 %i0, i64* %i0.addr, align 8
  store i64 %i1, i64* %i1.addr, align 8
  store i64 %i2, i64* %i2.addr, align 8
  store i64 %i3, i64* %i3.addr, align 8
  store i64 %i4, i64* %i4.addr, align 8
  store i64 %i5, i64* %i5.addr, align 8
  store i64 %i6, i64* %i6.addr, align 8
  store i64 %i7, i64* %i7.addr, align 8
  store %"class.std::__cxx11::basic_string"* %label, %"class.std::__cxx11::basic_string"** %label.addr, align 8
  ret void
}

; Function Attrs: noinline optnone uwtable
define linkonce_odr void @_ZNK6Kokkos4ViewIA2_PdJEE5labelB5cxx11Ev(%"class.std::__cxx11::basic_string"* noalias sret %agg.result, %"class.Kokkos::View"* %this) #2 comdat align 2 {
entry:
  %this.addr = alloca %"class.Kokkos::View"*, align 8
  store %"class.Kokkos::View"* %this, %"class.Kokkos::View"** %this.addr, align 8
  %this1 = load %"class.Kokkos::View"*, %"class.Kokkos::View"** %this.addr, align 8
  %m_track = getelementptr inbounds %"class.Kokkos::View", %"class.Kokkos::View"* %this1, i32 0, i32 0
  call void @_ZNK6Kokkos4Impl23SharedAllocationTracker9get_labelINS_9HostSpaceEEENSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEv(%"class.std::__cxx11::basic_string"* sret %agg.result, %"union.Kokkos::Impl::SharedAllocationTracker"* %m_track)
  ret void
}

; Function Attrs: nounwind
declare void @_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED1Ev(%"class.std::__cxx11::basic_string"*) unnamed_addr #6

; Function Attrs: nounwind
declare void @_ZNSaIcEC1Ev(%"class.std::allocator"*) unnamed_addr #6

declare void @_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EPKcRKS3_(%"class.std::__cxx11::basic_string"*, i8*, %"class.std::allocator"* dereferenceable(1)) unnamed_addr #1

; Function Attrs: noinline optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl12ViewCtorPropIJvNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEEEC2EOS7_(%"struct.Kokkos::Impl::ViewCtorProp.0"* %this, %"class.std::__cxx11::basic_string"* dereferenceable(32) %arg) unnamed_addr #2 comdat align 2 {
entry:
  %this.addr = alloca %"struct.Kokkos::Impl::ViewCtorProp.0"*, align 8
  %arg.addr = alloca %"class.std::__cxx11::basic_string"*, align 8
  store %"struct.Kokkos::Impl::ViewCtorProp.0"* %this, %"struct.Kokkos::Impl::ViewCtorProp.0"** %this.addr, align 8
  store %"class.std::__cxx11::basic_string"* %arg, %"class.std::__cxx11::basic_string"** %arg.addr, align 8
  %this1 = load %"struct.Kokkos::Impl::ViewCtorProp.0"*, %"struct.Kokkos::Impl::ViewCtorProp.0"** %this.addr, align 8
  %value = getelementptr inbounds %"struct.Kokkos::Impl::ViewCtorProp.0", %"struct.Kokkos::Impl::ViewCtorProp.0"* %this1, i32 0, i32 0
  %0 = load %"class.std::__cxx11::basic_string"*, %"class.std::__cxx11::basic_string"** %arg.addr, align 8
  call void @_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1ERKS4_(%"class.std::__cxx11::basic_string"* %value, %"class.std::__cxx11::basic_string"* dereferenceable(32) %0)
  ret void
}

; Function Attrs: nounwind
declare void @_ZNSaIcED1Ev(%"class.std::allocator"*) unnamed_addr #6

declare void @_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1ERKS4_(%"class.std::__cxx11::basic_string"*, %"class.std::__cxx11::basic_string"* dereferenceable(32)) unnamed_addr #1

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl11ViewMappingINS_10ViewTraitsIA2_PdJEEEJvEEC2Ev(%"class.Kokkos::Impl::ViewMapping"* %this) unnamed_addr #4 comdat align 2 {
entry:
  %this.addr = alloca %"class.Kokkos::Impl::ViewMapping"*, align 8
  store %"class.Kokkos::Impl::ViewMapping"* %this, %"class.Kokkos::Impl::ViewMapping"** %this.addr, align 8
  %this1 = load %"class.Kokkos::Impl::ViewMapping"*, %"class.Kokkos::Impl::ViewMapping"** %this.addr, align 8
  %m_handle = getelementptr inbounds %"class.Kokkos::Impl::ViewMapping", %"class.Kokkos::Impl::ViewMapping"* %this1, i32 0, i32 0
  store double* null, double** %m_handle, align 8
  %m_offset = getelementptr inbounds %"class.Kokkos::Impl::ViewMapping", %"class.Kokkos::Impl::ViewMapping"* %this1, i32 0, i32 1
  %0 = bitcast %"struct.Kokkos::Impl::ViewOffset"* %m_offset to i8*
  call void @llvm.memset.p0i8.i64(i8* %0, i8 0, i64 16, i32 8, i1 false)
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr zeroext i1 @_ZN6Kokkos6OpenMP14is_initializedEv() #4 comdat align 2 {
entry:
  %0 = load %"class.Kokkos::Impl::OpenMPExec"*, %"class.Kokkos::Impl::OpenMPExec"** @_ZN6Kokkos4Impl17t_openmp_instanceE, align 8
  %cmp = icmp ne %"class.Kokkos::Impl::OpenMPExec"* %0, null
  ret i1 %cmp
}

declare void @_ZN6Kokkos4Impl23throw_runtime_exceptionERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE(%"class.std::__cxx11::basic_string"* dereferenceable(32)) #1

; Function Attrs: noinline optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl12ViewCtorPropIJNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEESt17integral_constantIjLj0EENS_9HostSpaceENS_6OpenMPEEEC2IJS7_EEERKNS1_IJDpT_EEE(%"struct.Kokkos::Impl::ViewCtorProp.1"* %this, %"struct.Kokkos::Impl::ViewCtorProp"* dereferenceable(32) %arg) unnamed_addr #2 comdat align 2 personality i8* bitcast (i32 (...)* @__gxx_personality_v0 to i8*) {
entry:
  %this.addr = alloca %"struct.Kokkos::Impl::ViewCtorProp.1"*, align 8
  %arg.addr = alloca %"struct.Kokkos::Impl::ViewCtorProp"*, align 8
  %exn.slot = alloca i8*
  %ehselector.slot = alloca i32
  store %"struct.Kokkos::Impl::ViewCtorProp.1"* %this, %"struct.Kokkos::Impl::ViewCtorProp.1"** %this.addr, align 8
  store %"struct.Kokkos::Impl::ViewCtorProp"* %arg, %"struct.Kokkos::Impl::ViewCtorProp"** %arg.addr, align 8
  %this1 = load %"struct.Kokkos::Impl::ViewCtorProp.1"*, %"struct.Kokkos::Impl::ViewCtorProp.1"** %this.addr, align 8
  %0 = bitcast %"struct.Kokkos::Impl::ViewCtorProp.1"* %this1 to %"struct.Kokkos::Impl::ViewCtorProp.0"*
  %1 = load %"struct.Kokkos::Impl::ViewCtorProp"*, %"struct.Kokkos::Impl::ViewCtorProp"** %arg.addr, align 8
  %2 = bitcast %"struct.Kokkos::Impl::ViewCtorProp"* %1 to %"struct.Kokkos::Impl::ViewCtorProp.0"*
  call void @_ZN6Kokkos4Impl12ViewCtorPropIJvNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEEEC2ERKS8_(%"struct.Kokkos::Impl::ViewCtorProp.0"* %0, %"struct.Kokkos::Impl::ViewCtorProp.0"* dereferenceable(32) %2)
  %3 = bitcast %"struct.Kokkos::Impl::ViewCtorProp.1"* %this1 to %"struct.Kokkos::Impl::ViewCtorProp.2"*
  %4 = bitcast %"struct.Kokkos::Impl::ViewCtorProp.1"* %this1 to i8*
  %5 = getelementptr inbounds i8, i8* %4, i64 32
  %6 = bitcast i8* %5 to %"struct.Kokkos::Impl::ViewCtorProp.3"*
  invoke void @_ZN6Kokkos4Impl12ViewCtorPropIJvNS_9HostSpaceEEEC2Ev(%"struct.Kokkos::Impl::ViewCtorProp.3"* %6)
          to label %invoke.cont unwind label %lpad

invoke.cont:                                      ; preds = %entry
  %7 = bitcast %"struct.Kokkos::Impl::ViewCtorProp.1"* %this1 to i8*
  %8 = getelementptr inbounds i8, i8* %7, i64 36
  %9 = bitcast i8* %8 to %"struct.Kokkos::Impl::ViewCtorProp.4"*
  call void @_ZN6Kokkos4Impl12ViewCtorPropIJvNS_6OpenMPEEEC2Ev(%"struct.Kokkos::Impl::ViewCtorProp.4"* %9) #7
  ret void

lpad:                                             ; preds = %entry
  %10 = landingpad { i8*, i32 }
          cleanup
  %11 = extractvalue { i8*, i32 } %10, 0
  store i8* %11, i8** %exn.slot, align 8
  %12 = extractvalue { i8*, i32 } %10, 1
  store i32 %12, i32* %ehselector.slot, align 4
  %13 = bitcast %"struct.Kokkos::Impl::ViewCtorProp.1"* %this1 to %"struct.Kokkos::Impl::ViewCtorProp.0"*
  call void @_ZN6Kokkos4Impl12ViewCtorPropIJvNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEEED2Ev(%"struct.Kokkos::Impl::ViewCtorProp.0"* %13) #7
  br label %eh.resume

eh.resume:                                        ; preds = %lpad
  %exn = load i8*, i8** %exn.slot, align 8
  %sel = load i32, i32* %ehselector.slot, align 4
  %lpad.val = insertvalue { i8*, i32 } undef, i8* %exn, 0
  %lpad.val2 = insertvalue { i8*, i32 } %lpad.val, i32 %sel, 1
  resume { i8*, i32 } %lpad.val2
}

; Function Attrs: noinline optnone uwtable
define linkonce_odr %"class.Kokkos::Impl::SharedAllocationRecord"* @_ZN6Kokkos4Impl11ViewMappingINS_10ViewTraitsIA2_PdJEEEJvEE15allocate_sharedIJNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEESt17integral_constantIjLj0EENS_9HostSpaceENS_6OpenMPEEEEPNS0_22SharedAllocationRecordIvvEERKNS0_12ViewCtorPropIJDpT_EEERKNS_11LayoutRightE(%"class.Kokkos::Impl::ViewMapping"* %this, %"struct.Kokkos::Impl::ViewCtorProp.1"* dereferenceable(40) %arg_prop, %"struct.Kokkos::LayoutRight"* dereferenceable(64) %arg_layout) #2 comdat align 2 {
entry:
  %this.addr = alloca %"class.Kokkos::Impl::ViewMapping"*, align 8
  %arg_prop.addr = alloca %"struct.Kokkos::Impl::ViewCtorProp.1"*, align 8
  %arg_layout.addr = alloca %"struct.Kokkos::LayoutRight"*, align 8
  %ref.tmp = alloca %"struct.Kokkos::Impl::ViewOffset", align 8
  %ref.tmp2 = alloca %"struct.std::integral_constant", align 1
  %alloc_size = alloca i64, align 8
  %record = alloca %"class.Kokkos::Impl::SharedAllocationRecord.6"*, align 8
  %ref.tmp9 = alloca %"struct.Kokkos::Impl::ViewValueFunctor", align 8
  store %"class.Kokkos::Impl::ViewMapping"* %this, %"class.Kokkos::Impl::ViewMapping"** %this.addr, align 8
  store %"struct.Kokkos::Impl::ViewCtorProp.1"* %arg_prop, %"struct.Kokkos::Impl::ViewCtorProp.1"** %arg_prop.addr, align 8
  store %"struct.Kokkos::LayoutRight"* %arg_layout, %"struct.Kokkos::LayoutRight"** %arg_layout.addr, align 8
  %this1 = load %"class.Kokkos::Impl::ViewMapping"*, %"class.Kokkos::Impl::ViewMapping"** %this.addr, align 8
  %0 = load %"struct.Kokkos::LayoutRight"*, %"struct.Kokkos::LayoutRight"** %arg_layout.addr, align 8
  call void @_ZN6Kokkos4Impl10ViewOffsetINS0_13ViewDimensionIJLm0ELm2EEEENS_11LayoutRightEvEC2ILj0EEERKSt17integral_constantIjXT_EERKS4_(%"struct.Kokkos::Impl::ViewOffset"* %ref.tmp, %"struct.std::integral_constant"* dereferenceable(1) %ref.tmp2, %"struct.Kokkos::LayoutRight"* dereferenceable(64) %0)
  %m_offset = getelementptr inbounds %"class.Kokkos::Impl::ViewMapping", %"class.Kokkos::Impl::ViewMapping"* %this1, i32 0, i32 1
  %1 = bitcast %"struct.Kokkos::Impl::ViewOffset"* %m_offset to i8*
  %2 = bitcast %"struct.Kokkos::Impl::ViewOffset"* %ref.tmp to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %1, i8* %2, i64 16, i32 8, i1 false)
  %m_offset3 = getelementptr inbounds %"class.Kokkos::Impl::ViewMapping", %"class.Kokkos::Impl::ViewMapping"* %this1, i32 0, i32 1
  %call = call i64 @_ZNK6Kokkos4Impl10ViewOffsetINS0_13ViewDimensionIJLm0ELm2EEEENS_11LayoutRightEvE4spanEv(%"struct.Kokkos::Impl::ViewOffset"* %m_offset3)
  %mul = mul i64 %call, 8
  %add = add i64 %mul, 7
  %and = and i64 %add, -8
  store i64 %and, i64* %alloc_size, align 8
  %3 = load %"struct.Kokkos::Impl::ViewCtorProp.1"*, %"struct.Kokkos::Impl::ViewCtorProp.1"** %arg_prop.addr, align 8
  %4 = bitcast %"struct.Kokkos::Impl::ViewCtorProp.1"* %3 to i8*
  %add.ptr = getelementptr inbounds i8, i8* %4, i64 32
  %5 = bitcast i8* %add.ptr to %"struct.Kokkos::Impl::ViewCtorProp.3"*
  %value = getelementptr inbounds %"struct.Kokkos::Impl::ViewCtorProp.3", %"struct.Kokkos::Impl::ViewCtorProp.3"* %5, i32 0, i32 0
  %6 = load %"struct.Kokkos::Impl::ViewCtorProp.1"*, %"struct.Kokkos::Impl::ViewCtorProp.1"** %arg_prop.addr, align 8
  %7 = bitcast %"struct.Kokkos::Impl::ViewCtorProp.1"* %6 to %"struct.Kokkos::Impl::ViewCtorProp.0"*
  %value4 = getelementptr inbounds %"struct.Kokkos::Impl::ViewCtorProp.0", %"struct.Kokkos::Impl::ViewCtorProp.0"* %7, i32 0, i32 0
  %8 = load i64, i64* %alloc_size, align 8
  %call5 = call %"class.Kokkos::Impl::SharedAllocationRecord.6"* @_ZN6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceENS0_16ViewValueFunctorINS_6OpenMPEdLb1EEEE8allocateERKS2_RKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEm(%"class.Kokkos::HostSpace"* dereferenceable(4) %value, %"class.std::__cxx11::basic_string"* dereferenceable(32) %value4, i64 %8)
  store %"class.Kokkos::Impl::SharedAllocationRecord.6"* %call5, %"class.Kokkos::Impl::SharedAllocationRecord.6"** %record, align 8
  %9 = load i64, i64* %alloc_size, align 8
  %tobool = icmp ne i64 %9, 0
  br i1 %tobool, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %10 = load %"class.Kokkos::Impl::SharedAllocationRecord.6"*, %"class.Kokkos::Impl::SharedAllocationRecord.6"** %record, align 8
  %11 = bitcast %"class.Kokkos::Impl::SharedAllocationRecord.6"* %10 to %"class.Kokkos::Impl::SharedAllocationRecord"*
  %call6 = call i8* @_ZNK6Kokkos4Impl22SharedAllocationRecordIvvE4dataEv(%"class.Kokkos::Impl::SharedAllocationRecord"* %11)
  %12 = bitcast i8* %call6 to double*
  %m_handle = getelementptr inbounds %"class.Kokkos::Impl::ViewMapping", %"class.Kokkos::Impl::ViewMapping"* %this1, i32 0, i32 0
  store double* %12, double** %m_handle, align 8
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %13 = load i64, i64* %alloc_size, align 8
  %tobool7 = icmp ne i64 %13, 0
  br i1 %tobool7, label %if.then8, label %if.end16

if.then8:                                         ; preds = %if.end
  %14 = load %"struct.Kokkos::Impl::ViewCtorProp.1"*, %"struct.Kokkos::Impl::ViewCtorProp.1"** %arg_prop.addr, align 8
  %15 = bitcast %"struct.Kokkos::Impl::ViewCtorProp.1"* %14 to i8*
  %add.ptr10 = getelementptr inbounds i8, i8* %15, i64 36
  %16 = bitcast i8* %add.ptr10 to %"struct.Kokkos::Impl::ViewCtorProp.4"*
  %value11 = getelementptr inbounds %"struct.Kokkos::Impl::ViewCtorProp.4", %"struct.Kokkos::Impl::ViewCtorProp.4"* %16, i32 0, i32 0
  %m_handle12 = getelementptr inbounds %"class.Kokkos::Impl::ViewMapping", %"class.Kokkos::Impl::ViewMapping"* %this1, i32 0, i32 0
  %17 = load double*, double** %m_handle12, align 8
  %m_offset13 = getelementptr inbounds %"class.Kokkos::Impl::ViewMapping", %"class.Kokkos::Impl::ViewMapping"* %this1, i32 0, i32 1
  %call14 = call i64 @_ZNK6Kokkos4Impl10ViewOffsetINS0_13ViewDimensionIJLm0ELm2EEEENS_11LayoutRightEvE4spanEv(%"struct.Kokkos::Impl::ViewOffset"* %m_offset13)
  call void @_ZN6Kokkos4Impl16ViewValueFunctorINS_6OpenMPEdLb1EEC2ERKS2_Pdm(%"struct.Kokkos::Impl::ViewValueFunctor"* %ref.tmp9, %"class.Kokkos::OpenMP"* dereferenceable(1) %value11, double* %17, i64 %call14)
  %18 = load %"class.Kokkos::Impl::SharedAllocationRecord.6"*, %"class.Kokkos::Impl::SharedAllocationRecord.6"** %record, align 8
  %m_destroy = getelementptr inbounds %"class.Kokkos::Impl::SharedAllocationRecord.6", %"class.Kokkos::Impl::SharedAllocationRecord.6"* %18, i32 0, i32 1
  %19 = bitcast %"struct.Kokkos::Impl::ViewValueFunctor"* %m_destroy to i8*
  %20 = bitcast %"struct.Kokkos::Impl::ViewValueFunctor"* %ref.tmp9 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %19, i8* %20, i64 24, i32 8, i1 false)
  %21 = load %"class.Kokkos::Impl::SharedAllocationRecord.6"*, %"class.Kokkos::Impl::SharedAllocationRecord.6"** %record, align 8
  %m_destroy15 = getelementptr inbounds %"class.Kokkos::Impl::SharedAllocationRecord.6", %"class.Kokkos::Impl::SharedAllocationRecord.6"* %21, i32 0, i32 1
  call void @_ZN6Kokkos4Impl16ViewValueFunctorINS_6OpenMPEdLb1EE27construct_shared_allocationEv(%"struct.Kokkos::Impl::ViewValueFunctor"* %m_destroy15)
  br label %if.end16

if.end16:                                         ; preds = %if.then8, %if.end
  %22 = load %"class.Kokkos::Impl::SharedAllocationRecord.6"*, %"class.Kokkos::Impl::SharedAllocationRecord.6"** %record, align 8
  %23 = bitcast %"class.Kokkos::Impl::SharedAllocationRecord.6"* %22 to %"class.Kokkos::Impl::SharedAllocationRecord"*
  ret %"class.Kokkos::Impl::SharedAllocationRecord"* %23
}

; Function Attrs: noinline optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl23SharedAllocationTracker40assign_allocated_record_to_uninitializedEPNS0_22SharedAllocationRecordIvvEE(%"union.Kokkos::Impl::SharedAllocationTracker"* %this, %"class.Kokkos::Impl::SharedAllocationRecord"* %arg_record) #2 comdat align 2 {
entry:
  %this.addr = alloca %"union.Kokkos::Impl::SharedAllocationTracker"*, align 8
  %arg_record.addr = alloca %"class.Kokkos::Impl::SharedAllocationRecord"*, align 8
  store %"union.Kokkos::Impl::SharedAllocationTracker"* %this, %"union.Kokkos::Impl::SharedAllocationTracker"** %this.addr, align 8
  store %"class.Kokkos::Impl::SharedAllocationRecord"* %arg_record, %"class.Kokkos::Impl::SharedAllocationRecord"** %arg_record.addr, align 8
  %this1 = load %"union.Kokkos::Impl::SharedAllocationTracker"*, %"union.Kokkos::Impl::SharedAllocationTracker"** %this.addr, align 8
  %0 = load %"class.Kokkos::Impl::SharedAllocationRecord"*, %"class.Kokkos::Impl::SharedAllocationRecord"** %arg_record.addr, align 8
  %tobool = icmp ne %"class.Kokkos::Impl::SharedAllocationRecord"* %0, null
  br i1 %tobool, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %1 = load %"class.Kokkos::Impl::SharedAllocationRecord"*, %"class.Kokkos::Impl::SharedAllocationRecord"** %arg_record.addr, align 8
  %m_record = bitcast %"union.Kokkos::Impl::SharedAllocationTracker"* %this1 to %"class.Kokkos::Impl::SharedAllocationRecord"**
  store %"class.Kokkos::Impl::SharedAllocationRecord"* %1, %"class.Kokkos::Impl::SharedAllocationRecord"** %m_record, align 8
  call void @_ZN6Kokkos4Impl22SharedAllocationRecordIvvE9incrementEPS2_(%"class.Kokkos::Impl::SharedAllocationRecord"* %1)
  br label %if.end

if.else:                                          ; preds = %entry
  %m_record_bits = bitcast %"union.Kokkos::Impl::SharedAllocationTracker"* %this1 to i64*
  store i64 1, i64* %m_record_bits, align 8
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl12ViewCtorPropIJNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEESt17integral_constantIjLj0EENS_9HostSpaceENS_6OpenMPEEED2Ev(%"struct.Kokkos::Impl::ViewCtorProp.1"* %this) unnamed_addr #4 comdat align 2 {
entry:
  %this.addr = alloca %"struct.Kokkos::Impl::ViewCtorProp.1"*, align 8
  store %"struct.Kokkos::Impl::ViewCtorProp.1"* %this, %"struct.Kokkos::Impl::ViewCtorProp.1"** %this.addr, align 8
  %this1 = load %"struct.Kokkos::Impl::ViewCtorProp.1"*, %"struct.Kokkos::Impl::ViewCtorProp.1"** %this.addr, align 8
  %0 = bitcast %"struct.Kokkos::Impl::ViewCtorProp.1"* %this1 to %"struct.Kokkos::Impl::ViewCtorProp.0"*
  call void @_ZN6Kokkos4Impl12ViewCtorPropIJvNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEEED2Ev(%"struct.Kokkos::Impl::ViewCtorProp.0"* %0) #7
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl11ViewMappingINS_10ViewTraitsIA2_PdJEEEJvEED2Ev(%"class.Kokkos::Impl::ViewMapping"* %this) unnamed_addr #4 comdat align 2 {
entry:
  %this.addr = alloca %"class.Kokkos::Impl::ViewMapping"*, align 8
  store %"class.Kokkos::Impl::ViewMapping"* %this, %"class.Kokkos::Impl::ViewMapping"** %this.addr, align 8
  %this1 = load %"class.Kokkos::Impl::ViewMapping"*, %"class.Kokkos::Impl::ViewMapping"** %this.addr, align 8
  ret void
}

; Function Attrs: argmemonly nounwind
declare void @llvm.memset.p0i8.i64(i8* nocapture writeonly, i8, i64, i32, i1) #3

; Function Attrs: noinline optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl12ViewCtorPropIJvNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEEEC2ERKS8_(%"struct.Kokkos::Impl::ViewCtorProp.0"* %this, %"struct.Kokkos::Impl::ViewCtorProp.0"* dereferenceable(32)) unnamed_addr #2 comdat align 2 {
entry:
  %this.addr = alloca %"struct.Kokkos::Impl::ViewCtorProp.0"*, align 8
  %.addr = alloca %"struct.Kokkos::Impl::ViewCtorProp.0"*, align 8
  store %"struct.Kokkos::Impl::ViewCtorProp.0"* %this, %"struct.Kokkos::Impl::ViewCtorProp.0"** %this.addr, align 8
  store %"struct.Kokkos::Impl::ViewCtorProp.0"* %0, %"struct.Kokkos::Impl::ViewCtorProp.0"** %.addr, align 8
  %this1 = load %"struct.Kokkos::Impl::ViewCtorProp.0"*, %"struct.Kokkos::Impl::ViewCtorProp.0"** %this.addr, align 8
  %value = getelementptr inbounds %"struct.Kokkos::Impl::ViewCtorProp.0", %"struct.Kokkos::Impl::ViewCtorProp.0"* %this1, i32 0, i32 0
  %1 = load %"struct.Kokkos::Impl::ViewCtorProp.0"*, %"struct.Kokkos::Impl::ViewCtorProp.0"** %.addr, align 8
  %value2 = getelementptr inbounds %"struct.Kokkos::Impl::ViewCtorProp.0", %"struct.Kokkos::Impl::ViewCtorProp.0"* %1, i32 0, i32 0
  call void @_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1ERKS4_(%"class.std::__cxx11::basic_string"* %value, %"class.std::__cxx11::basic_string"* dereferenceable(32) %value2)
  ret void
}

; Function Attrs: noinline optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl12ViewCtorPropIJvNS_9HostSpaceEEEC2Ev(%"struct.Kokkos::Impl::ViewCtorProp.3"* %this) unnamed_addr #2 comdat align 2 {
entry:
  %this.addr = alloca %"struct.Kokkos::Impl::ViewCtorProp.3"*, align 8
  store %"struct.Kokkos::Impl::ViewCtorProp.3"* %this, %"struct.Kokkos::Impl::ViewCtorProp.3"** %this.addr, align 8
  %this1 = load %"struct.Kokkos::Impl::ViewCtorProp.3"*, %"struct.Kokkos::Impl::ViewCtorProp.3"** %this.addr, align 8
  %value = getelementptr inbounds %"struct.Kokkos::Impl::ViewCtorProp.3", %"struct.Kokkos::Impl::ViewCtorProp.3"* %this1, i32 0, i32 0
  call void @_ZN6Kokkos9HostSpaceC1Ev(%"class.Kokkos::HostSpace"* %value)
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl12ViewCtorPropIJvNS_6OpenMPEEEC2Ev(%"struct.Kokkos::Impl::ViewCtorProp.4"* %this) unnamed_addr #4 comdat align 2 {
entry:
  %this.addr = alloca %"struct.Kokkos::Impl::ViewCtorProp.4"*, align 8
  store %"struct.Kokkos::Impl::ViewCtorProp.4"* %this, %"struct.Kokkos::Impl::ViewCtorProp.4"** %this.addr, align 8
  %this1 = load %"struct.Kokkos::Impl::ViewCtorProp.4"*, %"struct.Kokkos::Impl::ViewCtorProp.4"** %this.addr, align 8
  %value = getelementptr inbounds %"struct.Kokkos::Impl::ViewCtorProp.4", %"struct.Kokkos::Impl::ViewCtorProp.4"* %this1, i32 0, i32 0
  call void @_ZN6Kokkos6OpenMPC2Ev(%"class.Kokkos::OpenMP"* %value) #7
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl12ViewCtorPropIJvNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEEED2Ev(%"struct.Kokkos::Impl::ViewCtorProp.0"* %this) unnamed_addr #4 comdat align 2 {
entry:
  %this.addr = alloca %"struct.Kokkos::Impl::ViewCtorProp.0"*, align 8
  store %"struct.Kokkos::Impl::ViewCtorProp.0"* %this, %"struct.Kokkos::Impl::ViewCtorProp.0"** %this.addr, align 8
  %this1 = load %"struct.Kokkos::Impl::ViewCtorProp.0"*, %"struct.Kokkos::Impl::ViewCtorProp.0"** %this.addr, align 8
  %value = getelementptr inbounds %"struct.Kokkos::Impl::ViewCtorProp.0", %"struct.Kokkos::Impl::ViewCtorProp.0"* %this1, i32 0, i32 0
  call void @_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED1Ev(%"class.std::__cxx11::basic_string"* %value) #7
  ret void
}

declare void @_ZN6Kokkos9HostSpaceC1Ev(%"class.Kokkos::HostSpace"*) unnamed_addr #1

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr void @_ZN6Kokkos6OpenMPC2Ev(%"class.Kokkos::OpenMP"* %this) unnamed_addr #4 comdat align 2 {
entry:
  %this.addr = alloca %"class.Kokkos::OpenMP"*, align 8
  store %"class.Kokkos::OpenMP"* %this, %"class.Kokkos::OpenMP"** %this.addr, align 8
  %this1 = load %"class.Kokkos::OpenMP"*, %"class.Kokkos::OpenMP"** %this.addr, align 8
  ret void
}

; Function Attrs: noinline optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl10ViewOffsetINS0_13ViewDimensionIJLm0ELm2EEEENS_11LayoutRightEvEC2ILj0EEERKSt17integral_constantIjXT_EERKS4_(%"struct.Kokkos::Impl::ViewOffset"* %this, %"struct.std::integral_constant"* dereferenceable(1), %"struct.Kokkos::LayoutRight"* dereferenceable(64) %arg_layout) unnamed_addr #2 comdat align 2 {
entry:
  %this.addr = alloca %"struct.Kokkos::Impl::ViewOffset"*, align 8
  %.addr = alloca %"struct.std::integral_constant"*, align 8
  %arg_layout.addr = alloca %"struct.Kokkos::LayoutRight"*, align 8
  store %"struct.Kokkos::Impl::ViewOffset"* %this, %"struct.Kokkos::Impl::ViewOffset"** %this.addr, align 8
  store %"struct.std::integral_constant"* %0, %"struct.std::integral_constant"** %.addr, align 8
  store %"struct.Kokkos::LayoutRight"* %arg_layout, %"struct.Kokkos::LayoutRight"** %arg_layout.addr, align 8
  %this1 = load %"struct.Kokkos::Impl::ViewOffset"*, %"struct.Kokkos::Impl::ViewOffset"** %this.addr, align 8
  %m_dim = getelementptr inbounds %"struct.Kokkos::Impl::ViewOffset", %"struct.Kokkos::Impl::ViewOffset"* %this1, i32 0, i32 0
  %1 = load %"struct.Kokkos::LayoutRight"*, %"struct.Kokkos::LayoutRight"** %arg_layout.addr, align 8
  %dimension = getelementptr inbounds %"struct.Kokkos::LayoutRight", %"struct.Kokkos::LayoutRight"* %1, i32 0, i32 0
  %arrayidx = getelementptr inbounds [8 x i64], [8 x i64]* %dimension, i64 0, i64 0
  %2 = load i64, i64* %arrayidx, align 8
  %3 = load %"struct.Kokkos::LayoutRight"*, %"struct.Kokkos::LayoutRight"** %arg_layout.addr, align 8
  %dimension2 = getelementptr inbounds %"struct.Kokkos::LayoutRight", %"struct.Kokkos::LayoutRight"* %3, i32 0, i32 0
  %arrayidx3 = getelementptr inbounds [8 x i64], [8 x i64]* %dimension2, i64 0, i64 1
  %4 = load i64, i64* %arrayidx3, align 8
  %5 = load %"struct.Kokkos::LayoutRight"*, %"struct.Kokkos::LayoutRight"** %arg_layout.addr, align 8
  %dimension4 = getelementptr inbounds %"struct.Kokkos::LayoutRight", %"struct.Kokkos::LayoutRight"* %5, i32 0, i32 0
  %arrayidx5 = getelementptr inbounds [8 x i64], [8 x i64]* %dimension4, i64 0, i64 2
  %6 = load i64, i64* %arrayidx5, align 8
  %7 = load %"struct.Kokkos::LayoutRight"*, %"struct.Kokkos::LayoutRight"** %arg_layout.addr, align 8
  %dimension6 = getelementptr inbounds %"struct.Kokkos::LayoutRight", %"struct.Kokkos::LayoutRight"* %7, i32 0, i32 0
  %arrayidx7 = getelementptr inbounds [8 x i64], [8 x i64]* %dimension6, i64 0, i64 3
  %8 = load i64, i64* %arrayidx7, align 8
  %9 = load %"struct.Kokkos::LayoutRight"*, %"struct.Kokkos::LayoutRight"** %arg_layout.addr, align 8
  %dimension8 = getelementptr inbounds %"struct.Kokkos::LayoutRight", %"struct.Kokkos::LayoutRight"* %9, i32 0, i32 0
  %arrayidx9 = getelementptr inbounds [8 x i64], [8 x i64]* %dimension8, i64 0, i64 4
  %10 = load i64, i64* %arrayidx9, align 8
  %11 = load %"struct.Kokkos::LayoutRight"*, %"struct.Kokkos::LayoutRight"** %arg_layout.addr, align 8
  %dimension10 = getelementptr inbounds %"struct.Kokkos::LayoutRight", %"struct.Kokkos::LayoutRight"* %11, i32 0, i32 0
  %arrayidx11 = getelementptr inbounds [8 x i64], [8 x i64]* %dimension10, i64 0, i64 5
  %12 = load i64, i64* %arrayidx11, align 8
  %13 = load %"struct.Kokkos::LayoutRight"*, %"struct.Kokkos::LayoutRight"** %arg_layout.addr, align 8
  %dimension12 = getelementptr inbounds %"struct.Kokkos::LayoutRight", %"struct.Kokkos::LayoutRight"* %13, i32 0, i32 0
  %arrayidx13 = getelementptr inbounds [8 x i64], [8 x i64]* %dimension12, i64 0, i64 6
  %14 = load i64, i64* %arrayidx13, align 8
  %15 = load %"struct.Kokkos::LayoutRight"*, %"struct.Kokkos::LayoutRight"** %arg_layout.addr, align 8
  %dimension14 = getelementptr inbounds %"struct.Kokkos::LayoutRight", %"struct.Kokkos::LayoutRight"* %15, i32 0, i32 0
  %arrayidx15 = getelementptr inbounds [8 x i64], [8 x i64]* %dimension14, i64 0, i64 7
  %16 = load i64, i64* %arrayidx15, align 8
  call void @_ZN6Kokkos4Impl13ViewDimensionIJLm0ELm2EEEC2Emmmmmmmm(%"struct.Kokkos::Impl::ViewDimension"* %m_dim, i64 %2, i64 %4, i64 %6, i64 %8, i64 %10, i64 %12, i64 %14, i64 %16)
  %m_stride = getelementptr inbounds %"struct.Kokkos::Impl::ViewOffset", %"struct.Kokkos::Impl::ViewOffset"* %this1, i32 0, i32 1
  %m_dim16 = getelementptr inbounds %"struct.Kokkos::Impl::ViewOffset", %"struct.Kokkos::Impl::ViewOffset"* %this1, i32 0, i32 0
  %call = call i64 @_ZN6Kokkos4Impl10ViewOffsetINS0_13ViewDimensionIJLm0ELm2EEEENS_11LayoutRightEvE7PaddingILj0EE6strideEm(i64 2)
  store i64 %call, i64* %m_stride, align 8
  ret void
}

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture writeonly, i8* nocapture readonly, i64, i32, i1) #3

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr i64 @_ZNK6Kokkos4Impl10ViewOffsetINS0_13ViewDimensionIJLm0ELm2EEEENS_11LayoutRightEvE4spanEv(%"struct.Kokkos::Impl::ViewOffset"* %this) #4 comdat align 2 {
entry:
  %this.addr = alloca %"struct.Kokkos::Impl::ViewOffset"*, align 8
  store %"struct.Kokkos::Impl::ViewOffset"* %this, %"struct.Kokkos::Impl::ViewOffset"** %this.addr, align 8
  %this1 = load %"struct.Kokkos::Impl::ViewOffset"*, %"struct.Kokkos::Impl::ViewOffset"** %this.addr, align 8
  %m_dim = getelementptr inbounds %"struct.Kokkos::Impl::ViewOffset", %"struct.Kokkos::Impl::ViewOffset"* %this1, i32 0, i32 0
  %0 = bitcast %"struct.Kokkos::Impl::ViewDimension"* %m_dim to %"struct.Kokkos::Impl::ViewDimension0"*
  %N0 = getelementptr inbounds %"struct.Kokkos::Impl::ViewDimension0", %"struct.Kokkos::Impl::ViewDimension0"* %0, i32 0, i32 0
  %1 = load i64, i64* %N0, align 8
  %m_stride = getelementptr inbounds %"struct.Kokkos::Impl::ViewOffset", %"struct.Kokkos::Impl::ViewOffset"* %this1, i32 0, i32 1
  %2 = load i64, i64* %m_stride, align 8
  %mul = mul i64 %1, %2
  ret i64 %mul
}

; Function Attrs: noinline optnone uwtable
define linkonce_odr %"class.Kokkos::Impl::SharedAllocationRecord.6"* @_ZN6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceENS0_16ViewValueFunctorINS_6OpenMPEdLb1EEEE8allocateERKS2_RKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEm(%"class.Kokkos::HostSpace"* dereferenceable(4) %arg_space, %"class.std::__cxx11::basic_string"* dereferenceable(32) %arg_label, i64 %arg_alloc) #2 comdat align 2 personality i8* bitcast (i32 (...)* @__gxx_personality_v0 to i8*) {
entry:
  %arg_space.addr = alloca %"class.Kokkos::HostSpace"*, align 8
  %arg_label.addr = alloca %"class.std::__cxx11::basic_string"*, align 8
  %arg_alloc.addr = alloca i64, align 8
  %exn.slot = alloca i8*
  %ehselector.slot = alloca i32
  store %"class.Kokkos::HostSpace"* %arg_space, %"class.Kokkos::HostSpace"** %arg_space.addr, align 8
  store %"class.std::__cxx11::basic_string"* %arg_label, %"class.std::__cxx11::basic_string"** %arg_label.addr, align 8
  store i64 %arg_alloc, i64* %arg_alloc.addr, align 8
  %call = call i8* @_Znwm(i64 64) #15
  %0 = bitcast i8* %call to %"class.Kokkos::Impl::SharedAllocationRecord.6"*
  %1 = load %"class.Kokkos::HostSpace"*, %"class.Kokkos::HostSpace"** %arg_space.addr, align 8
  %2 = load %"class.std::__cxx11::basic_string"*, %"class.std::__cxx11::basic_string"** %arg_label.addr, align 8
  %3 = load i64, i64* %arg_alloc.addr, align 8
  invoke void @_ZN6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceENS0_16ViewValueFunctorINS_6OpenMPEdLb1EEEEC2ERKS2_RKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEm(%"class.Kokkos::Impl::SharedAllocationRecord.6"* %0, %"class.Kokkos::HostSpace"* dereferenceable(4) %1, %"class.std::__cxx11::basic_string"* dereferenceable(32) %2, i64 %3)
          to label %invoke.cont unwind label %lpad

invoke.cont:                                      ; preds = %entry
  ret %"class.Kokkos::Impl::SharedAllocationRecord.6"* %0

lpad:                                             ; preds = %entry
  %4 = landingpad { i8*, i32 }
          cleanup
  %5 = extractvalue { i8*, i32 } %4, 0
  store i8* %5, i8** %exn.slot, align 8
  %6 = extractvalue { i8*, i32 } %4, 1
  store i32 %6, i32* %ehselector.slot, align 4
  call void @_ZdlPv(i8* %call) #16
  br label %eh.resume

eh.resume:                                        ; preds = %lpad
  %exn = load i8*, i8** %exn.slot, align 8
  %sel = load i32, i32* %ehselector.slot, align 4
  %lpad.val = insertvalue { i8*, i32 } undef, i8* %exn, 0
  %lpad.val1 = insertvalue { i8*, i32 } %lpad.val, i32 %sel, 1
  resume { i8*, i32 } %lpad.val1
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr i8* @_ZNK6Kokkos4Impl22SharedAllocationRecordIvvE4dataEv(%"class.Kokkos::Impl::SharedAllocationRecord"* %this) #4 comdat align 2 {
entry:
  %this.addr = alloca %"class.Kokkos::Impl::SharedAllocationRecord"*, align 8
  store %"class.Kokkos::Impl::SharedAllocationRecord"* %this, %"class.Kokkos::Impl::SharedAllocationRecord"** %this.addr, align 8
  %this1 = load %"class.Kokkos::Impl::SharedAllocationRecord"*, %"class.Kokkos::Impl::SharedAllocationRecord"** %this.addr, align 8
  %m_alloc_ptr = getelementptr inbounds %"class.Kokkos::Impl::SharedAllocationRecord", %"class.Kokkos::Impl::SharedAllocationRecord"* %this1, i32 0, i32 1
  %0 = load %"class.Kokkos::Impl::SharedAllocationHeader"*, %"class.Kokkos::Impl::SharedAllocationHeader"** %m_alloc_ptr, align 8
  %add.ptr = getelementptr inbounds %"class.Kokkos::Impl::SharedAllocationHeader", %"class.Kokkos::Impl::SharedAllocationHeader"* %0, i64 1
  %1 = bitcast %"class.Kokkos::Impl::SharedAllocationHeader"* %add.ptr to i8*
  ret i8* %1
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl16ViewValueFunctorINS_6OpenMPEdLb1EEC2ERKS2_Pdm(%"struct.Kokkos::Impl::ViewValueFunctor"* %this, %"class.Kokkos::OpenMP"* dereferenceable(1) %arg_space, double* %arg_ptr, i64 %arg_n) unnamed_addr #4 comdat align 2 {
entry:
  %this.addr = alloca %"struct.Kokkos::Impl::ViewValueFunctor"*, align 8
  %arg_space.addr = alloca %"class.Kokkos::OpenMP"*, align 8
  %arg_ptr.addr = alloca double*, align 8
  %arg_n.addr = alloca i64, align 8
  store %"struct.Kokkos::Impl::ViewValueFunctor"* %this, %"struct.Kokkos::Impl::ViewValueFunctor"** %this.addr, align 8
  store %"class.Kokkos::OpenMP"* %arg_space, %"class.Kokkos::OpenMP"** %arg_space.addr, align 8
  store double* %arg_ptr, double** %arg_ptr.addr, align 8
  store i64 %arg_n, i64* %arg_n.addr, align 8
  %this1 = load %"struct.Kokkos::Impl::ViewValueFunctor"*, %"struct.Kokkos::Impl::ViewValueFunctor"** %this.addr, align 8
  %space = getelementptr inbounds %"struct.Kokkos::Impl::ViewValueFunctor", %"struct.Kokkos::Impl::ViewValueFunctor"* %this1, i32 0, i32 0
  %0 = load %"class.Kokkos::OpenMP"*, %"class.Kokkos::OpenMP"** %arg_space.addr, align 8
  %ptr = getelementptr inbounds %"struct.Kokkos::Impl::ViewValueFunctor", %"struct.Kokkos::Impl::ViewValueFunctor"* %this1, i32 0, i32 1
  %1 = load double*, double** %arg_ptr.addr, align 8
  store double* %1, double** %ptr, align 8
  %n = getelementptr inbounds %"struct.Kokkos::Impl::ViewValueFunctor", %"struct.Kokkos::Impl::ViewValueFunctor"* %this1, i32 0, i32 2
  %2 = load i64, i64* %arg_n.addr, align 8
  store i64 %2, i64* %n, align 8
  ret void
}

; Function Attrs: noinline optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl16ViewValueFunctorINS_6OpenMPEdLb1EE27construct_shared_allocationEv(%"struct.Kokkos::Impl::ViewValueFunctor"* %this) #2 comdat align 2 personality i8* bitcast (i32 (...)* @__gxx_personality_v0 to i8*) {
entry:
  %this.addr = alloca %"struct.Kokkos::Impl::ViewValueFunctor"*, align 8
  %ref.tmp = alloca %"class.Kokkos::OpenMP", align 1
  %kpID = alloca i64, align 8
  %ref.tmp4 = alloca %"class.std::__cxx11::basic_string", align 8
  %ref.tmp5 = alloca %"class.std::allocator", align 1
  %exn.slot = alloca i8*
  %ehselector.slot = alloca i32
  %closure = alloca %"class.Kokkos::Impl::ParallelFor", align 8
  %agg.tmp = alloca %"class.Kokkos::RangePolicy", align 8
  %ref.tmp9 = alloca %"class.Kokkos::OpenMP", align 1
  %i = alloca i64, align 8
  store %"struct.Kokkos::Impl::ViewValueFunctor"* %this, %"struct.Kokkos::Impl::ViewValueFunctor"** %this.addr, align 8
  %this1 = load %"struct.Kokkos::Impl::ViewValueFunctor"*, %"struct.Kokkos::Impl::ViewValueFunctor"** %this.addr, align 8
  %space = getelementptr inbounds %"struct.Kokkos::Impl::ViewValueFunctor", %"struct.Kokkos::Impl::ViewValueFunctor"* %this1, i32 0, i32 0
  call void @_ZN6Kokkos6OpenMPC2Ev(%"class.Kokkos::OpenMP"* %ref.tmp) #7
  %call = call zeroext i1 @_ZN6Kokkos6OpenMP11in_parallelERKS0_(%"class.Kokkos::OpenMP"* dereferenceable(1) %ref.tmp) #7
  %lnot = xor i1 %call, true
  br i1 %lnot, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  store i64 0, i64* %kpID, align 8
  %call2 = call zeroext i1 @_ZN6Kokkos9Profiling20profileLibraryLoadedEv()
  br i1 %call2, label %if.then3, label %if.end

if.then3:                                         ; preds = %if.then
  call void @_ZNSaIcEC1Ev(%"class.std::allocator"* %ref.tmp5) #7
  invoke void @_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EPKcRKS3_(%"class.std::__cxx11::basic_string"* %ref.tmp4, i8* getelementptr inbounds ([29 x i8], [29 x i8]* @.str.5, i32 0, i32 0), %"class.std::allocator"* dereferenceable(1) %ref.tmp5)
          to label %invoke.cont unwind label %lpad

invoke.cont:                                      ; preds = %if.then3
  invoke void @_ZN6Kokkos9Profiling16beginParallelForERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEjPm(%"class.std::__cxx11::basic_string"* dereferenceable(32) %ref.tmp4, i32 0, i64* %kpID)
          to label %invoke.cont7 unwind label %lpad6

invoke.cont7:                                     ; preds = %invoke.cont
  call void @_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED1Ev(%"class.std::__cxx11::basic_string"* %ref.tmp4) #7
  call void @_ZNSaIcED1Ev(%"class.std::allocator"* %ref.tmp5) #7
  br label %if.end

lpad:                                             ; preds = %if.then3
  %0 = landingpad { i8*, i32 }
          cleanup
  %1 = extractvalue { i8*, i32 } %0, 0
  store i8* %1, i8** %exn.slot, align 8
  %2 = extractvalue { i8*, i32 } %0, 1
  store i32 %2, i32* %ehselector.slot, align 4
  br label %ehcleanup

lpad6:                                            ; preds = %invoke.cont
  %3 = landingpad { i8*, i32 }
          cleanup
  %4 = extractvalue { i8*, i32 } %3, 0
  store i8* %4, i8** %exn.slot, align 8
  %5 = extractvalue { i8*, i32 } %3, 1
  store i32 %5, i32* %ehselector.slot, align 4
  call void @_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED1Ev(%"class.std::__cxx11::basic_string"* %ref.tmp4) #7
  br label %ehcleanup

ehcleanup:                                        ; preds = %lpad6, %lpad
  call void @_ZNSaIcED1Ev(%"class.std::allocator"* %ref.tmp5) #7
  br label %eh.resume

if.end:                                           ; preds = %invoke.cont7, %if.then
  %n = getelementptr inbounds %"struct.Kokkos::Impl::ViewValueFunctor", %"struct.Kokkos::Impl::ViewValueFunctor"* %this1, i32 0, i32 2
  %6 = load i64, i64* %n, align 8
  call void @_ZN6Kokkos11RangePolicyIJNS_6OpenMPEEEC2Emm(%"class.Kokkos::RangePolicy"* %agg.tmp, i64 0, i64 %6)
  call void @_ZN6Kokkos4Impl11ParallelForINS0_16ViewValueFunctorINS_6OpenMPEdLb1EEENS_11RangePolicyIJS3_EEES3_EC2ERKS4_S6_(%"class.Kokkos::Impl::ParallelFor"* %closure, %"struct.Kokkos::Impl::ViewValueFunctor"* dereferenceable(24) %this1, %"class.Kokkos::RangePolicy"* byval align 8 %agg.tmp)
  call void @_ZNK6Kokkos4Impl11ParallelForINS0_16ViewValueFunctorINS_6OpenMPEdLb1EEENS_11RangePolicyIJS3_EEES3_E7executeEv(%"class.Kokkos::Impl::ParallelFor"* %closure)
  %space8 = getelementptr inbounds %"struct.Kokkos::Impl::ViewValueFunctor", %"struct.Kokkos::Impl::ViewValueFunctor"* %this1, i32 0, i32 0
  call void @_ZN6Kokkos6OpenMPC2Ev(%"class.Kokkos::OpenMP"* %ref.tmp9) #7
  call void @_ZN6Kokkos6OpenMP5fenceERKS0_(%"class.Kokkos::OpenMP"* dereferenceable(1) %ref.tmp9) #7
  %call10 = call zeroext i1 @_ZN6Kokkos9Profiling20profileLibraryLoadedEv()
  br i1 %call10, label %if.then11, label %if.end12

if.then11:                                        ; preds = %if.end
  %7 = load i64, i64* %kpID, align 8
  call void @_ZN6Kokkos9Profiling14endParallelForEm(i64 %7)
  br label %if.end12

if.end12:                                         ; preds = %if.then11, %if.end
  br label %if.end14

if.else:                                          ; preds = %entry
  store i64 0, i64* %i, align 8
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %if.else
  %8 = load i64, i64* %i, align 8
  %n13 = getelementptr inbounds %"struct.Kokkos::Impl::ViewValueFunctor", %"struct.Kokkos::Impl::ViewValueFunctor"* %this1, i32 0, i32 2
  %9 = load i64, i64* %n13, align 8
  %cmp = icmp ult i64 %8, %9
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %10 = load i64, i64* %i, align 8
  call void @_ZNK6Kokkos4Impl16ViewValueFunctorINS_6OpenMPEdLb1EEclEm(%"struct.Kokkos::Impl::ViewValueFunctor"* %this1, i64 %10)
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %11 = load i64, i64* %i, align 8
  %inc = add i64 %11, 1
  store i64 %inc, i64* %i, align 8
  br label %for.cond

for.end:                                          ; preds = %for.cond
  br label %if.end14

if.end14:                                         ; preds = %for.end, %if.end12
  ret void

eh.resume:                                        ; preds = %ehcleanup
  %exn = load i8*, i8** %exn.slot, align 8
  %sel = load i32, i32* %ehselector.slot, align 4
  %lpad.val = insertvalue { i8*, i32 } undef, i8* %exn, 0
  %lpad.val15 = insertvalue { i8*, i32 } %lpad.val, i32 %sel, 1
  resume { i8*, i32 } %lpad.val15
}

; Function Attrs: noinline optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl13ViewDimensionIJLm0ELm2EEEC2Emmmmmmmm(%"struct.Kokkos::Impl::ViewDimension"* %this, i64 %n0, i64 %n1, i64 %n2, i64 %n3, i64 %n4, i64 %n5, i64 %n6, i64 %n7) unnamed_addr #2 comdat align 2 {
entry:
  %this.addr = alloca %"struct.Kokkos::Impl::ViewDimension"*, align 8
  %n0.addr = alloca i64, align 8
  %n1.addr = alloca i64, align 8
  %n2.addr = alloca i64, align 8
  %n3.addr = alloca i64, align 8
  %n4.addr = alloca i64, align 8
  %n5.addr = alloca i64, align 8
  %n6.addr = alloca i64, align 8
  %n7.addr = alloca i64, align 8
  store %"struct.Kokkos::Impl::ViewDimension"* %this, %"struct.Kokkos::Impl::ViewDimension"** %this.addr, align 8
  store i64 %n0, i64* %n0.addr, align 8
  store i64 %n1, i64* %n1.addr, align 8
  store i64 %n2, i64* %n2.addr, align 8
  store i64 %n3, i64* %n3.addr, align 8
  store i64 %n4, i64* %n4.addr, align 8
  store i64 %n5, i64* %n5.addr, align 8
  store i64 %n6, i64* %n6.addr, align 8
  store i64 %n7, i64* %n7.addr, align 8
  %this1 = load %"struct.Kokkos::Impl::ViewDimension"*, %"struct.Kokkos::Impl::ViewDimension"** %this.addr, align 8
  %0 = bitcast %"struct.Kokkos::Impl::ViewDimension"* %this1 to %"struct.Kokkos::Impl::ViewDimension0"*
  %1 = load i64, i64* %n0.addr, align 8
  call void @_ZN6Kokkos4Impl14ViewDimension0ILm0ELj1EEC2Em(%"struct.Kokkos::Impl::ViewDimension0"* %0, i64 %1)
  %2 = bitcast %"struct.Kokkos::Impl::ViewDimension"* %this1 to %"struct.Kokkos::Impl::ViewDimension1"*
  %3 = load i64, i64* %n1.addr, align 8
  call void @_ZN6Kokkos4Impl14ViewDimension1ILm2ELj1EEC2Em(%"struct.Kokkos::Impl::ViewDimension1"* %2, i64 %3)
  %4 = bitcast %"struct.Kokkos::Impl::ViewDimension"* %this1 to %"struct.Kokkos::Impl::ViewDimension2"*
  %5 = load i64, i64* %n2.addr, align 8
  call void @_ZN6Kokkos4Impl14ViewDimension2ILm18446744073709551615ELj1EEC2Em(%"struct.Kokkos::Impl::ViewDimension2"* %4, i64 %5)
  %6 = bitcast %"struct.Kokkos::Impl::ViewDimension"* %this1 to %"struct.Kokkos::Impl::ViewDimension3"*
  %7 = load i64, i64* %n3.addr, align 8
  call void @_ZN6Kokkos4Impl14ViewDimension3ILm18446744073709551615ELj1EEC2Em(%"struct.Kokkos::Impl::ViewDimension3"* %6, i64 %7)
  %8 = bitcast %"struct.Kokkos::Impl::ViewDimension"* %this1 to %"struct.Kokkos::Impl::ViewDimension4"*
  %9 = load i64, i64* %n4.addr, align 8
  call void @_ZN6Kokkos4Impl14ViewDimension4ILm18446744073709551615ELj1EEC2Em(%"struct.Kokkos::Impl::ViewDimension4"* %8, i64 %9)
  %10 = bitcast %"struct.Kokkos::Impl::ViewDimension"* %this1 to %"struct.Kokkos::Impl::ViewDimension5"*
  %11 = load i64, i64* %n5.addr, align 8
  call void @_ZN6Kokkos4Impl14ViewDimension5ILm18446744073709551615ELj1EEC2Em(%"struct.Kokkos::Impl::ViewDimension5"* %10, i64 %11)
  %12 = bitcast %"struct.Kokkos::Impl::ViewDimension"* %this1 to %"struct.Kokkos::Impl::ViewDimension6"*
  %13 = load i64, i64* %n6.addr, align 8
  call void @_ZN6Kokkos4Impl14ViewDimension6ILm18446744073709551615ELj1EEC2Em(%"struct.Kokkos::Impl::ViewDimension6"* %12, i64 %13)
  %14 = bitcast %"struct.Kokkos::Impl::ViewDimension"* %this1 to %"struct.Kokkos::Impl::ViewDimension7"*
  %15 = load i64, i64* %n7.addr, align 8
  call void @_ZN6Kokkos4Impl14ViewDimension7ILm18446744073709551615ELj1EEC2Em(%"struct.Kokkos::Impl::ViewDimension7"* %14, i64 %15)
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr i64 @_ZN6Kokkos4Impl10ViewOffsetINS0_13ViewDimensionIJLm0ELm2EEEENS_11LayoutRightEvE7PaddingILj0EE6strideEm(i64 %N) #4 comdat align 2 {
entry:
  %N.addr = alloca i64, align 8
  store i64 %N, i64* %N.addr, align 8
  %0 = load i64, i64* %N.addr, align 8
  ret i64 %0
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl14ViewDimension0ILm0ELj1EEC2Em(%"struct.Kokkos::Impl::ViewDimension0"* %this, i64 %V) unnamed_addr #4 comdat align 2 {
entry:
  %this.addr = alloca %"struct.Kokkos::Impl::ViewDimension0"*, align 8
  %V.addr = alloca i64, align 8
  store %"struct.Kokkos::Impl::ViewDimension0"* %this, %"struct.Kokkos::Impl::ViewDimension0"** %this.addr, align 8
  store i64 %V, i64* %V.addr, align 8
  %this1 = load %"struct.Kokkos::Impl::ViewDimension0"*, %"struct.Kokkos::Impl::ViewDimension0"** %this.addr, align 8
  %N0 = getelementptr inbounds %"struct.Kokkos::Impl::ViewDimension0", %"struct.Kokkos::Impl::ViewDimension0"* %this1, i32 0, i32 0
  %0 = load i64, i64* %V.addr, align 8
  store i64 %0, i64* %N0, align 8
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl14ViewDimension1ILm2ELj1EEC2Em(%"struct.Kokkos::Impl::ViewDimension1"* %this, i64) unnamed_addr #4 comdat align 2 {
entry:
  %this.addr = alloca %"struct.Kokkos::Impl::ViewDimension1"*, align 8
  %.addr = alloca i64, align 8
  store %"struct.Kokkos::Impl::ViewDimension1"* %this, %"struct.Kokkos::Impl::ViewDimension1"** %this.addr, align 8
  store i64 %0, i64* %.addr, align 8
  %this1 = load %"struct.Kokkos::Impl::ViewDimension1"*, %"struct.Kokkos::Impl::ViewDimension1"** %this.addr, align 8
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl14ViewDimension2ILm18446744073709551615ELj1EEC2Em(%"struct.Kokkos::Impl::ViewDimension2"* %this, i64) unnamed_addr #4 comdat align 2 {
entry:
  %this.addr = alloca %"struct.Kokkos::Impl::ViewDimension2"*, align 8
  %.addr = alloca i64, align 8
  store %"struct.Kokkos::Impl::ViewDimension2"* %this, %"struct.Kokkos::Impl::ViewDimension2"** %this.addr, align 8
  store i64 %0, i64* %.addr, align 8
  %this1 = load %"struct.Kokkos::Impl::ViewDimension2"*, %"struct.Kokkos::Impl::ViewDimension2"** %this.addr, align 8
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl14ViewDimension3ILm18446744073709551615ELj1EEC2Em(%"struct.Kokkos::Impl::ViewDimension3"* %this, i64) unnamed_addr #4 comdat align 2 {
entry:
  %this.addr = alloca %"struct.Kokkos::Impl::ViewDimension3"*, align 8
  %.addr = alloca i64, align 8
  store %"struct.Kokkos::Impl::ViewDimension3"* %this, %"struct.Kokkos::Impl::ViewDimension3"** %this.addr, align 8
  store i64 %0, i64* %.addr, align 8
  %this1 = load %"struct.Kokkos::Impl::ViewDimension3"*, %"struct.Kokkos::Impl::ViewDimension3"** %this.addr, align 8
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl14ViewDimension4ILm18446744073709551615ELj1EEC2Em(%"struct.Kokkos::Impl::ViewDimension4"* %this, i64) unnamed_addr #4 comdat align 2 {
entry:
  %this.addr = alloca %"struct.Kokkos::Impl::ViewDimension4"*, align 8
  %.addr = alloca i64, align 8
  store %"struct.Kokkos::Impl::ViewDimension4"* %this, %"struct.Kokkos::Impl::ViewDimension4"** %this.addr, align 8
  store i64 %0, i64* %.addr, align 8
  %this1 = load %"struct.Kokkos::Impl::ViewDimension4"*, %"struct.Kokkos::Impl::ViewDimension4"** %this.addr, align 8
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl14ViewDimension5ILm18446744073709551615ELj1EEC2Em(%"struct.Kokkos::Impl::ViewDimension5"* %this, i64) unnamed_addr #4 comdat align 2 {
entry:
  %this.addr = alloca %"struct.Kokkos::Impl::ViewDimension5"*, align 8
  %.addr = alloca i64, align 8
  store %"struct.Kokkos::Impl::ViewDimension5"* %this, %"struct.Kokkos::Impl::ViewDimension5"** %this.addr, align 8
  store i64 %0, i64* %.addr, align 8
  %this1 = load %"struct.Kokkos::Impl::ViewDimension5"*, %"struct.Kokkos::Impl::ViewDimension5"** %this.addr, align 8
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl14ViewDimension6ILm18446744073709551615ELj1EEC2Em(%"struct.Kokkos::Impl::ViewDimension6"* %this, i64) unnamed_addr #4 comdat align 2 {
entry:
  %this.addr = alloca %"struct.Kokkos::Impl::ViewDimension6"*, align 8
  %.addr = alloca i64, align 8
  store %"struct.Kokkos::Impl::ViewDimension6"* %this, %"struct.Kokkos::Impl::ViewDimension6"** %this.addr, align 8
  store i64 %0, i64* %.addr, align 8
  %this1 = load %"struct.Kokkos::Impl::ViewDimension6"*, %"struct.Kokkos::Impl::ViewDimension6"** %this.addr, align 8
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl14ViewDimension7ILm18446744073709551615ELj1EEC2Em(%"struct.Kokkos::Impl::ViewDimension7"* %this, i64) unnamed_addr #4 comdat align 2 {
entry:
  %this.addr = alloca %"struct.Kokkos::Impl::ViewDimension7"*, align 8
  %.addr = alloca i64, align 8
  store %"struct.Kokkos::Impl::ViewDimension7"* %this, %"struct.Kokkos::Impl::ViewDimension7"** %this.addr, align 8
  store i64 %0, i64* %.addr, align 8
  %this1 = load %"struct.Kokkos::Impl::ViewDimension7"*, %"struct.Kokkos::Impl::ViewDimension7"** %this.addr, align 8
  ret void
}

; Function Attrs: nobuiltin
declare noalias nonnull i8* @_Znwm(i64) #8

; Function Attrs: noinline optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceENS0_16ViewValueFunctorINS_6OpenMPEdLb1EEEEC2ERKS2_RKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEm(%"class.Kokkos::Impl::SharedAllocationRecord.6"* %this, %"class.Kokkos::HostSpace"* dereferenceable(4) %arg_space, %"class.std::__cxx11::basic_string"* dereferenceable(32) %arg_label, i64 %arg_alloc) unnamed_addr #2 comdat align 2 {
entry:
  %this.addr = alloca %"class.Kokkos::Impl::SharedAllocationRecord.6"*, align 8
  %arg_space.addr = alloca %"class.Kokkos::HostSpace"*, align 8
  %arg_label.addr = alloca %"class.std::__cxx11::basic_string"*, align 8
  %arg_alloc.addr = alloca i64, align 8
  store %"class.Kokkos::Impl::SharedAllocationRecord.6"* %this, %"class.Kokkos::Impl::SharedAllocationRecord.6"** %this.addr, align 8
  store %"class.Kokkos::HostSpace"* %arg_space, %"class.Kokkos::HostSpace"** %arg_space.addr, align 8
  store %"class.std::__cxx11::basic_string"* %arg_label, %"class.std::__cxx11::basic_string"** %arg_label.addr, align 8
  store i64 %arg_alloc, i64* %arg_alloc.addr, align 8
  %this1 = load %"class.Kokkos::Impl::SharedAllocationRecord.6"*, %"class.Kokkos::Impl::SharedAllocationRecord.6"** %this.addr, align 8
  %0 = bitcast %"class.Kokkos::Impl::SharedAllocationRecord.6"* %this1 to %"class.Kokkos::Impl::SharedAllocationRecord.7"*
  %1 = load %"class.Kokkos::HostSpace"*, %"class.Kokkos::HostSpace"** %arg_space.addr, align 8
  %2 = load %"class.std::__cxx11::basic_string"*, %"class.std::__cxx11::basic_string"** %arg_label.addr, align 8
  %3 = load i64, i64* %arg_alloc.addr, align 8
  call void @_ZN6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceEvEC2ERKS2_RKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEmPFvPNS1_IvvEEE(%"class.Kokkos::Impl::SharedAllocationRecord.7"* %0, %"class.Kokkos::HostSpace"* dereferenceable(4) %1, %"class.std::__cxx11::basic_string"* dereferenceable(32) %2, i64 %3, void (%"class.Kokkos::Impl::SharedAllocationRecord"*)* @_ZN6Kokkos4Impl12_GLOBAL__N_110deallocateINS_9HostSpaceENS0_16ViewValueFunctorINS_6OpenMPEdLb1EEEEEvPNS0_22SharedAllocationRecordIvvEE)
  %4 = bitcast %"class.Kokkos::Impl::SharedAllocationRecord.6"* %this1 to i32 (...)***
  store i32 (...)** bitcast (i8** getelementptr inbounds ({ [5 x i8*] }, { [5 x i8*] }* @_ZTVN6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceENS0_16ViewValueFunctorINS_6OpenMPEdLb1EEEEE, i32 0, inrange i32 0, i32 2) to i32 (...)**), i32 (...)*** %4, align 8
  %m_destroy = getelementptr inbounds %"class.Kokkos::Impl::SharedAllocationRecord.6", %"class.Kokkos::Impl::SharedAllocationRecord.6"* %this1, i32 0, i32 1
  %5 = bitcast %"struct.Kokkos::Impl::ViewValueFunctor"* %m_destroy to i8*
  call void @llvm.memset.p0i8.i64(i8* %5, i8 0, i64 24, i32 8, i1 false)
  call void @_ZN6Kokkos4Impl16ViewValueFunctorINS_6OpenMPEdLb1EEC2Ev(%"struct.Kokkos::Impl::ViewValueFunctor"* %m_destroy) #7
  ret void
}

; Function Attrs: nobuiltin nounwind
declare void @_ZdlPv(i8*) #9

; Function Attrs: noinline optnone uwtable
define internal void @_ZN6Kokkos4Impl12_GLOBAL__N_110deallocateINS_9HostSpaceENS0_16ViewValueFunctorINS_6OpenMPEdLb1EEEEEvPNS0_22SharedAllocationRecordIvvEE(%"class.Kokkos::Impl::SharedAllocationRecord"* %record_ptr) #2 {
entry:
  %record_ptr.addr = alloca %"class.Kokkos::Impl::SharedAllocationRecord"*, align 8
  %ptr = alloca %"class.Kokkos::Impl::SharedAllocationRecord.6"*, align 8
  store %"class.Kokkos::Impl::SharedAllocationRecord"* %record_ptr, %"class.Kokkos::Impl::SharedAllocationRecord"** %record_ptr.addr, align 8
  %0 = load %"class.Kokkos::Impl::SharedAllocationRecord"*, %"class.Kokkos::Impl::SharedAllocationRecord"** %record_ptr.addr, align 8
  %1 = bitcast %"class.Kokkos::Impl::SharedAllocationRecord"* %0 to %"class.Kokkos::Impl::SharedAllocationRecord.7"*
  %2 = bitcast %"class.Kokkos::Impl::SharedAllocationRecord.7"* %1 to %"class.Kokkos::Impl::SharedAllocationRecord.6"*
  store %"class.Kokkos::Impl::SharedAllocationRecord.6"* %2, %"class.Kokkos::Impl::SharedAllocationRecord.6"** %ptr, align 8
  %3 = load %"class.Kokkos::Impl::SharedAllocationRecord.6"*, %"class.Kokkos::Impl::SharedAllocationRecord.6"** %ptr, align 8
  %m_destroy = getelementptr inbounds %"class.Kokkos::Impl::SharedAllocationRecord.6", %"class.Kokkos::Impl::SharedAllocationRecord.6"* %3, i32 0, i32 1
  call void @_ZN6Kokkos4Impl16ViewValueFunctorINS_6OpenMPEdLb1EE25destroy_shared_allocationEv(%"struct.Kokkos::Impl::ViewValueFunctor"* %m_destroy)
  %4 = load %"class.Kokkos::Impl::SharedAllocationRecord.6"*, %"class.Kokkos::Impl::SharedAllocationRecord.6"** %ptr, align 8
  %isnull = icmp eq %"class.Kokkos::Impl::SharedAllocationRecord.6"* %4, null
  br i1 %isnull, label %delete.end, label %delete.notnull

delete.notnull:                                   ; preds = %entry
  %5 = bitcast %"class.Kokkos::Impl::SharedAllocationRecord.6"* %4 to void (%"class.Kokkos::Impl::SharedAllocationRecord.6"*)***
  %vtable = load void (%"class.Kokkos::Impl::SharedAllocationRecord.6"*)**, void (%"class.Kokkos::Impl::SharedAllocationRecord.6"*)*** %5, align 8
  %vfn = getelementptr inbounds void (%"class.Kokkos::Impl::SharedAllocationRecord.6"*)*, void (%"class.Kokkos::Impl::SharedAllocationRecord.6"*)** %vtable, i64 2
  %6 = load void (%"class.Kokkos::Impl::SharedAllocationRecord.6"*)*, void (%"class.Kokkos::Impl::SharedAllocationRecord.6"*)** %vfn, align 8
  call void %6(%"class.Kokkos::Impl::SharedAllocationRecord.6"* %4) #7
  br label %delete.end

delete.end:                                       ; preds = %delete.notnull, %entry
  ret void
}

declare void @_ZN6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceEvEC2ERKS2_RKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEmPFvPNS1_IvvEEE(%"class.Kokkos::Impl::SharedAllocationRecord.7"*, %"class.Kokkos::HostSpace"* dereferenceable(4), %"class.std::__cxx11::basic_string"* dereferenceable(32), i64, void (%"class.Kokkos::Impl::SharedAllocationRecord"*)*) unnamed_addr #1

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl16ViewValueFunctorINS_6OpenMPEdLb1EEC2Ev(%"struct.Kokkos::Impl::ViewValueFunctor"* %this) unnamed_addr #4 comdat align 2 {
entry:
  %this.addr = alloca %"struct.Kokkos::Impl::ViewValueFunctor"*, align 8
  store %"struct.Kokkos::Impl::ViewValueFunctor"* %this, %"struct.Kokkos::Impl::ViewValueFunctor"** %this.addr, align 8
  %this1 = load %"struct.Kokkos::Impl::ViewValueFunctor"*, %"struct.Kokkos::Impl::ViewValueFunctor"** %this.addr, align 8
  %space = getelementptr inbounds %"struct.Kokkos::Impl::ViewValueFunctor", %"struct.Kokkos::Impl::ViewValueFunctor"* %this1, i32 0, i32 0
  call void @_ZN6Kokkos6OpenMPC2Ev(%"class.Kokkos::OpenMP"* %space) #7
  ret void
}

; Function Attrs: noinline optnone uwtable
define linkonce_odr void @_ZNK6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceEvE9get_labelB5cxx11Ev(%"class.std::__cxx11::basic_string"* noalias sret %agg.result, %"class.Kokkos::Impl::SharedAllocationRecord.7"* %this) unnamed_addr #2 comdat align 2 personality i8* bitcast (i32 (...)* @__gxx_personality_v0 to i8*) {
entry:
  %this.addr = alloca %"class.Kokkos::Impl::SharedAllocationRecord.7"*, align 8
  %ref.tmp = alloca %"class.std::allocator", align 1
  %exn.slot = alloca i8*
  %ehselector.slot = alloca i32
  store %"class.Kokkos::Impl::SharedAllocationRecord.7"* %this, %"class.Kokkos::Impl::SharedAllocationRecord.7"** %this.addr, align 8
  %this1 = load %"class.Kokkos::Impl::SharedAllocationRecord.7"*, %"class.Kokkos::Impl::SharedAllocationRecord.7"** %this.addr, align 8
  %0 = bitcast %"class.Kokkos::Impl::SharedAllocationRecord.7"* %this1 to %"class.Kokkos::Impl::SharedAllocationRecord"*
  %call = call %"class.Kokkos::Impl::SharedAllocationHeader"* @_ZNK6Kokkos4Impl22SharedAllocationRecordIvvE4headEv(%"class.Kokkos::Impl::SharedAllocationRecord"* %0)
  %m_label = getelementptr inbounds %"class.Kokkos::Impl::SharedAllocationHeader", %"class.Kokkos::Impl::SharedAllocationHeader"* %call, i32 0, i32 1
  %arraydecay = getelementptr inbounds [120 x i8], [120 x i8]* %m_label, i32 0, i32 0
  call void @_ZNSaIcEC1Ev(%"class.std::allocator"* %ref.tmp) #7
  invoke void @_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EPKcRKS3_(%"class.std::__cxx11::basic_string"* %agg.result, i8* %arraydecay, %"class.std::allocator"* dereferenceable(1) %ref.tmp)
          to label %invoke.cont unwind label %lpad

invoke.cont:                                      ; preds = %entry
  call void @_ZNSaIcED1Ev(%"class.std::allocator"* %ref.tmp) #7
  ret void

lpad:                                             ; preds = %entry
  %1 = landingpad { i8*, i32 }
          cleanup
  %2 = extractvalue { i8*, i32 } %1, 0
  store i8* %2, i8** %exn.slot, align 8
  %3 = extractvalue { i8*, i32 } %1, 1
  store i32 %3, i32* %ehselector.slot, align 4
  call void @_ZNSaIcED1Ev(%"class.std::allocator"* %ref.tmp) #7
  br label %eh.resume

eh.resume:                                        ; preds = %lpad
  %exn = load i8*, i8** %exn.slot, align 8
  %sel = load i32, i32* %ehselector.slot, align 4
  %lpad.val = insertvalue { i8*, i32 } undef, i8* %exn, 0
  %lpad.val2 = insertvalue { i8*, i32 } %lpad.val, i32 %sel, 1
  resume { i8*, i32 } %lpad.val2
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceENS0_16ViewValueFunctorINS_6OpenMPEdLb1EEEED2Ev(%"class.Kokkos::Impl::SharedAllocationRecord.6"* %this) unnamed_addr #4 comdat align 2 {
entry:
  %this.addr = alloca %"class.Kokkos::Impl::SharedAllocationRecord.6"*, align 8
  store %"class.Kokkos::Impl::SharedAllocationRecord.6"* %this, %"class.Kokkos::Impl::SharedAllocationRecord.6"** %this.addr, align 8
  %this1 = load %"class.Kokkos::Impl::SharedAllocationRecord.6"*, %"class.Kokkos::Impl::SharedAllocationRecord.6"** %this.addr, align 8
  %0 = bitcast %"class.Kokkos::Impl::SharedAllocationRecord.6"* %this1 to %"class.Kokkos::Impl::SharedAllocationRecord.7"*
  call void @_ZN6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceEvED2Ev(%"class.Kokkos::Impl::SharedAllocationRecord.7"* %0) #7
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceENS0_16ViewValueFunctorINS_6OpenMPEdLb1EEEED0Ev(%"class.Kokkos::Impl::SharedAllocationRecord.6"* %this) unnamed_addr #4 comdat align 2 {
entry:
  %this.addr = alloca %"class.Kokkos::Impl::SharedAllocationRecord.6"*, align 8
  store %"class.Kokkos::Impl::SharedAllocationRecord.6"* %this, %"class.Kokkos::Impl::SharedAllocationRecord.6"** %this.addr, align 8
  %this1 = load %"class.Kokkos::Impl::SharedAllocationRecord.6"*, %"class.Kokkos::Impl::SharedAllocationRecord.6"** %this.addr, align 8
  call void @_ZN6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceENS0_16ViewValueFunctorINS_6OpenMPEdLb1EEEED2Ev(%"class.Kokkos::Impl::SharedAllocationRecord.6"* %this1) #7
  %0 = bitcast %"class.Kokkos::Impl::SharedAllocationRecord.6"* %this1 to i8*
  call void @_ZdlPv(i8* %0) #16
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl16ViewValueFunctorINS_6OpenMPEdLb1EE25destroy_shared_allocationEv(%"struct.Kokkos::Impl::ViewValueFunctor"* %this) #4 comdat align 2 {
entry:
  %this.addr = alloca %"struct.Kokkos::Impl::ViewValueFunctor"*, align 8
  store %"struct.Kokkos::Impl::ViewValueFunctor"* %this, %"struct.Kokkos::Impl::ViewValueFunctor"** %this.addr, align 8
  %this1 = load %"struct.Kokkos::Impl::ViewValueFunctor"*, %"struct.Kokkos::Impl::ViewValueFunctor"** %this.addr, align 8
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr %"class.Kokkos::Impl::SharedAllocationHeader"* @_ZNK6Kokkos4Impl22SharedAllocationRecordIvvE4headEv(%"class.Kokkos::Impl::SharedAllocationRecord"* %this) #4 comdat align 2 {
entry:
  %this.addr = alloca %"class.Kokkos::Impl::SharedAllocationRecord"*, align 8
  store %"class.Kokkos::Impl::SharedAllocationRecord"* %this, %"class.Kokkos::Impl::SharedAllocationRecord"** %this.addr, align 8
  %this1 = load %"class.Kokkos::Impl::SharedAllocationRecord"*, %"class.Kokkos::Impl::SharedAllocationRecord"** %this.addr, align 8
  %m_alloc_ptr = getelementptr inbounds %"class.Kokkos::Impl::SharedAllocationRecord", %"class.Kokkos::Impl::SharedAllocationRecord"* %this1, i32 0, i32 1
  %0 = load %"class.Kokkos::Impl::SharedAllocationHeader"*, %"class.Kokkos::Impl::SharedAllocationHeader"** %m_alloc_ptr, align 8
  ret %"class.Kokkos::Impl::SharedAllocationHeader"* %0
}

; Function Attrs: nounwind
declare void @_ZN6Kokkos4Impl22SharedAllocationRecordINS_9HostSpaceEvED2Ev(%"class.Kokkos::Impl::SharedAllocationRecord.7"*) unnamed_addr #6

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr zeroext i1 @_ZN6Kokkos6OpenMP11in_parallelERKS0_(%"class.Kokkos::OpenMP"* dereferenceable(1)) #4 comdat align 2 personality i8* bitcast (i32 (...)* @__gxx_personality_v0 to i8*) {
entry:
  %.addr = alloca %"class.Kokkos::OpenMP"*, align 8
  store %"class.Kokkos::OpenMP"* %0, %"class.Kokkos::OpenMP"** %.addr, align 8
  %1 = load %"class.Kokkos::Impl::OpenMPExec"*, %"class.Kokkos::Impl::OpenMPExec"** @_ZN6Kokkos4Impl17t_openmp_instanceE, align 8
  %tobool = icmp ne %"class.Kokkos::Impl::OpenMPExec"* %1, null
  br i1 %tobool, label %lor.rhs, label %lor.end

lor.rhs:                                          ; preds = %entry
  %2 = load %"class.Kokkos::Impl::OpenMPExec"*, %"class.Kokkos::Impl::OpenMPExec"** @_ZN6Kokkos4Impl17t_openmp_instanceE, align 8
  %m_level = getelementptr inbounds %"class.Kokkos::Impl::OpenMPExec", %"class.Kokkos::Impl::OpenMPExec"* %2, i32 0, i32 1
  %3 = load i32, i32* %m_level, align 4
  %call = invoke i32 @omp_get_level()
          to label %invoke.cont unwind label %terminate.lpad

invoke.cont:                                      ; preds = %lor.rhs
  %cmp = icmp slt i32 %3, %call
  br label %lor.end

lor.end:                                          ; preds = %invoke.cont, %entry
  %4 = phi i1 [ true, %entry ], [ %cmp, %invoke.cont ]
  ret i1 %4

terminate.lpad:                                   ; preds = %lor.rhs
  %5 = landingpad { i8*, i32 }
          catch i8* null
  %6 = extractvalue { i8*, i32 } %5, 0
  call void @__clang_call_terminate(i8* %6) #14
  unreachable
}

declare zeroext i1 @_ZN6Kokkos9Profiling20profileLibraryLoadedEv() #1

declare void @_ZN6Kokkos9Profiling16beginParallelForERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEjPm(%"class.std::__cxx11::basic_string"* dereferenceable(32), i32, i64*) #1

; Function Attrs: noinline optnone uwtable
define linkonce_odr void @_ZN6Kokkos11RangePolicyIJNS_6OpenMPEEEC2Emm(%"class.Kokkos::RangePolicy"* %this, i64 %work_begin, i64 %work_end) unnamed_addr #2 comdat align 2 {
entry:
  %this.addr = alloca %"class.Kokkos::RangePolicy"*, align 8
  %work_begin.addr = alloca i64, align 8
  %work_end.addr = alloca i64, align 8
  %ref.tmp = alloca %"class.Kokkos::OpenMP", align 1
  store %"class.Kokkos::RangePolicy"* %this, %"class.Kokkos::RangePolicy"** %this.addr, align 8
  store i64 %work_begin, i64* %work_begin.addr, align 8
  store i64 %work_end, i64* %work_end.addr, align 8
  %this1 = load %"class.Kokkos::RangePolicy"*, %"class.Kokkos::RangePolicy"** %this.addr, align 8
  call void @_ZN6Kokkos6OpenMPC2Ev(%"class.Kokkos::OpenMP"* %ref.tmp) #7
  %0 = load i64, i64* %work_begin.addr, align 8
  %1 = load i64, i64* %work_end.addr, align 8
  call void @_ZN6Kokkos11RangePolicyIJNS_6OpenMPEEEC2ERKS1_mm(%"class.Kokkos::RangePolicy"* %this1, %"class.Kokkos::OpenMP"* dereferenceable(1) %ref.tmp, i64 %0, i64 %1)
  call void @_ZN6Kokkos11RangePolicyIJNS_6OpenMPEEE19set_auto_chunk_sizeEv(%"class.Kokkos::RangePolicy"* %this1)
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl11ParallelForINS0_16ViewValueFunctorINS_6OpenMPEdLb1EEENS_11RangePolicyIJS3_EEES3_EC2ERKS4_S6_(%"class.Kokkos::Impl::ParallelFor"* %this, %"struct.Kokkos::Impl::ViewValueFunctor"* dereferenceable(24) %arg_functor, %"class.Kokkos::RangePolicy"* byval align 8 %arg_policy) unnamed_addr #4 comdat align 2 {
entry:
  %this.addr = alloca %"class.Kokkos::Impl::ParallelFor"*, align 8
  %arg_functor.addr = alloca %"struct.Kokkos::Impl::ViewValueFunctor"*, align 8
  store %"class.Kokkos::Impl::ParallelFor"* %this, %"class.Kokkos::Impl::ParallelFor"** %this.addr, align 8
  store %"struct.Kokkos::Impl::ViewValueFunctor"* %arg_functor, %"struct.Kokkos::Impl::ViewValueFunctor"** %arg_functor.addr, align 8
  %this1 = load %"class.Kokkos::Impl::ParallelFor"*, %"class.Kokkos::Impl::ParallelFor"** %this.addr, align 8
  %m_instance = getelementptr inbounds %"class.Kokkos::Impl::ParallelFor", %"class.Kokkos::Impl::ParallelFor"* %this1, i32 0, i32 0
  %0 = load %"class.Kokkos::Impl::OpenMPExec"*, %"class.Kokkos::Impl::OpenMPExec"** @_ZN6Kokkos4Impl17t_openmp_instanceE, align 8
  store %"class.Kokkos::Impl::OpenMPExec"* %0, %"class.Kokkos::Impl::OpenMPExec"** %m_instance, align 8
  %m_functor = getelementptr inbounds %"class.Kokkos::Impl::ParallelFor", %"class.Kokkos::Impl::ParallelFor"* %this1, i32 0, i32 1
  %1 = load %"struct.Kokkos::Impl::ViewValueFunctor"*, %"struct.Kokkos::Impl::ViewValueFunctor"** %arg_functor.addr, align 8
  %2 = bitcast %"struct.Kokkos::Impl::ViewValueFunctor"* %m_functor to i8*
  %3 = bitcast %"struct.Kokkos::Impl::ViewValueFunctor"* %1 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %2, i8* %3, i64 24, i32 8, i1 false)
  %m_policy = getelementptr inbounds %"class.Kokkos::Impl::ParallelFor", %"class.Kokkos::Impl::ParallelFor"* %this1, i32 0, i32 2
  %4 = bitcast %"class.Kokkos::RangePolicy"* %m_policy to i8*
  %5 = bitcast %"class.Kokkos::RangePolicy"* %arg_policy to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %4, i8* %5, i64 40, i32 8, i1 false)
  ret void
}

; Function Attrs: noinline optnone uwtable
define linkonce_odr void @_ZNK6Kokkos4Impl11ParallelForINS0_16ViewValueFunctorINS_6OpenMPEdLb1EEENS_11RangePolicyIJS3_EEES3_E7executeEv(%"class.Kokkos::Impl::ParallelFor"* %this) #2 comdat align 2 {
entry:
  %this.addr = alloca %"class.Kokkos::Impl::ParallelFor"*, align 8
  %ref.tmp = alloca %"class.Kokkos::OpenMP", align 1
  %pool_size = alloca i32, align 4
  %0 = call i32 @__kmpc_global_thread_num(%ident_t* @0)
  store %"class.Kokkos::Impl::ParallelFor"* %this, %"class.Kokkos::Impl::ParallelFor"** %this.addr, align 8
  %this1 = load %"class.Kokkos::Impl::ParallelFor"*, %"class.Kokkos::Impl::ParallelFor"** %this.addr, align 8
  call void @_ZN6Kokkos6OpenMPC2Ev(%"class.Kokkos::OpenMP"* %ref.tmp) #7
  %call = call zeroext i1 @_ZN6Kokkos6OpenMP11in_parallelERKS0_(%"class.Kokkos::OpenMP"* dereferenceable(1) %ref.tmp) #7
  br i1 %call, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %m_functor = getelementptr inbounds %"class.Kokkos::Impl::ParallelFor", %"class.Kokkos::Impl::ParallelFor"* %this1, i32 0, i32 1
  %m_policy = getelementptr inbounds %"class.Kokkos::Impl::ParallelFor", %"class.Kokkos::Impl::ParallelFor"* %this1, i32 0, i32 2
  %call2 = call i64 @_ZNK6Kokkos11RangePolicyIJNS_6OpenMPEEE5beginEv(%"class.Kokkos::RangePolicy"* %m_policy)
  %m_policy3 = getelementptr inbounds %"class.Kokkos::Impl::ParallelFor", %"class.Kokkos::Impl::ParallelFor"* %this1, i32 0, i32 2
  %call4 = call i64 @_ZNK6Kokkos11RangePolicyIJNS_6OpenMPEEE3endEv(%"class.Kokkos::RangePolicy"* %m_policy3)
  call void @_ZN6Kokkos4Impl11ParallelForINS0_16ViewValueFunctorINS_6OpenMPEdLb1EEENS_11RangePolicyIJS3_EEES3_E10exec_rangeIvEENSt9enable_ifIXsr3std7is_sameIT_vEE5valueEvE4typeERKS4_mm(%"struct.Kokkos::Impl::ViewValueFunctor"* dereferenceable(24) %m_functor, i64 %call2, i64 %call4)
  br label %if.end

if.else:                                          ; preds = %entry
  call void @_ZN6Kokkos4Impl10OpenMPExec16verify_is_masterEPKc(i8* getelementptr inbounds ([28 x i8], [28 x i8]* @.str.7, i32 0, i32 0))
  %call5 = call i32 @_ZN6Kokkos6OpenMP16thread_pool_sizeEv() #7
  store i32 %call5, i32* %pool_size, align 4
  %1 = load i32, i32* %pool_size, align 4
  call void @__kmpc_push_num_threads(%ident_t* @0, i32 %0, i32 %1)
  call void (%ident_t*, i32, void (i32*, i32*, ...)*, ...) @__kmpc_fork_call(%ident_t* @0, i32 1, void (i32*, i32*, ...)* bitcast (void (i32*, i32*, %"class.Kokkos::Impl::ParallelFor"*)* @.omp_outlined. to void (i32*, i32*, ...)*), %"class.Kokkos::Impl::ParallelFor"* %this1)
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr void @_ZN6Kokkos6OpenMP5fenceERKS0_(%"class.Kokkos::OpenMP"* dereferenceable(1) %instance) #4 comdat align 2 {
entry:
  %instance.addr = alloca %"class.Kokkos::OpenMP"*, align 8
  store %"class.Kokkos::OpenMP"* %instance, %"class.Kokkos::OpenMP"** %instance.addr, align 8
  ret void
}

declare void @_ZN6Kokkos9Profiling14endParallelForEm(i64) #1

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr void @_ZNK6Kokkos4Impl16ViewValueFunctorINS_6OpenMPEdLb1EEclEm(%"struct.Kokkos::Impl::ViewValueFunctor"* %this, i64 %i) #4 comdat align 2 {
entry:
  %this.addr = alloca %"struct.Kokkos::Impl::ViewValueFunctor"*, align 8
  %i.addr = alloca i64, align 8
  store %"struct.Kokkos::Impl::ViewValueFunctor"* %this, %"struct.Kokkos::Impl::ViewValueFunctor"** %this.addr, align 8
  store i64 %i, i64* %i.addr, align 8
  %this1 = load %"struct.Kokkos::Impl::ViewValueFunctor"*, %"struct.Kokkos::Impl::ViewValueFunctor"** %this.addr, align 8
  %ptr = getelementptr inbounds %"struct.Kokkos::Impl::ViewValueFunctor", %"struct.Kokkos::Impl::ViewValueFunctor"* %this1, i32 0, i32 1
  %0 = load double*, double** %ptr, align 8
  %1 = load i64, i64* %i.addr, align 8
  %arrayidx = getelementptr inbounds double, double* %0, i64 %1
  store double 0.000000e+00, double* %arrayidx, align 8
  ret void
}

declare i32 @omp_get_level() #1

; Function Attrs: noinline noreturn nounwind
define linkonce_odr hidden void @__clang_call_terminate(i8*) #10 comdat {
  %2 = call i8* @__cxa_begin_catch(i8* %0) #7
  call void @_ZSt9terminatev() #14
  unreachable
}

declare i8* @__cxa_begin_catch(i8*)

declare void @_ZSt9terminatev()

; Function Attrs: noinline optnone uwtable
define linkonce_odr void @_ZN6Kokkos11RangePolicyIJNS_6OpenMPEEEC2ERKS1_mm(%"class.Kokkos::RangePolicy"* %this, %"class.Kokkos::OpenMP"* dereferenceable(1) %work_space, i64 %work_begin, i64 %work_end) unnamed_addr #2 comdat align 2 {
entry:
  %this.addr = alloca %"class.Kokkos::RangePolicy"*, align 8
  %work_space.addr = alloca %"class.Kokkos::OpenMP"*, align 8
  %work_begin.addr = alloca i64, align 8
  %work_end.addr = alloca i64, align 8
  store %"class.Kokkos::RangePolicy"* %this, %"class.Kokkos::RangePolicy"** %this.addr, align 8
  store %"class.Kokkos::OpenMP"* %work_space, %"class.Kokkos::OpenMP"** %work_space.addr, align 8
  store i64 %work_begin, i64* %work_begin.addr, align 8
  store i64 %work_end, i64* %work_end.addr, align 8
  %this1 = load %"class.Kokkos::RangePolicy"*, %"class.Kokkos::RangePolicy"** %this.addr, align 8
  %0 = bitcast %"class.Kokkos::RangePolicy"* %this1 to %"struct.Kokkos::Impl::PolicyTraits"*
  %m_space = getelementptr inbounds %"class.Kokkos::RangePolicy", %"class.Kokkos::RangePolicy"* %this1, i32 0, i32 0
  %1 = load %"class.Kokkos::OpenMP"*, %"class.Kokkos::OpenMP"** %work_space.addr, align 8
  %m_begin = getelementptr inbounds %"class.Kokkos::RangePolicy", %"class.Kokkos::RangePolicy"* %this1, i32 0, i32 1
  %2 = load i64, i64* %work_begin.addr, align 8
  %3 = load i64, i64* %work_end.addr, align 8
  %cmp = icmp ult i64 %2, %3
  br i1 %cmp, label %cond.true, label %cond.false

cond.true:                                        ; preds = %entry
  %4 = load i64, i64* %work_begin.addr, align 8
  br label %cond.end

cond.false:                                       ; preds = %entry
  br label %cond.end

cond.end:                                         ; preds = %cond.false, %cond.true
  %cond = phi i64 [ %4, %cond.true ], [ 0, %cond.false ]
  store i64 %cond, i64* %m_begin, align 8
  %m_end = getelementptr inbounds %"class.Kokkos::RangePolicy", %"class.Kokkos::RangePolicy"* %this1, i32 0, i32 2
  %5 = load i64, i64* %work_begin.addr, align 8
  %6 = load i64, i64* %work_end.addr, align 8
  %cmp2 = icmp ult i64 %5, %6
  br i1 %cmp2, label %cond.true3, label %cond.false4

cond.true3:                                       ; preds = %cond.end
  %7 = load i64, i64* %work_end.addr, align 8
  br label %cond.end5

cond.false4:                                      ; preds = %cond.end
  br label %cond.end5

cond.end5:                                        ; preds = %cond.false4, %cond.true3
  %cond6 = phi i64 [ %7, %cond.true3 ], [ 0, %cond.false4 ]
  store i64 %cond6, i64* %m_end, align 8
  %m_granularity = getelementptr inbounds %"class.Kokkos::RangePolicy", %"class.Kokkos::RangePolicy"* %this1, i32 0, i32 3
  store i64 0, i64* %m_granularity, align 8
  %m_granularity_mask = getelementptr inbounds %"class.Kokkos::RangePolicy", %"class.Kokkos::RangePolicy"* %this1, i32 0, i32 4
  store i64 0, i64* %m_granularity_mask, align 8
  call void @_ZN6Kokkos11RangePolicyIJNS_6OpenMPEEE19set_auto_chunk_sizeEv(%"class.Kokkos::RangePolicy"* %this1)
  ret void
}

; Function Attrs: noinline optnone uwtable
define linkonce_odr void @_ZN6Kokkos11RangePolicyIJNS_6OpenMPEEE19set_auto_chunk_sizeEv(%"class.Kokkos::RangePolicy"* %this) #2 comdat align 2 {
entry:
  %this.addr = alloca %"class.Kokkos::RangePolicy"*, align 8
  %concurrency = alloca i64, align 8
  %new_chunk_size = alloca i64, align 8
  store %"class.Kokkos::RangePolicy"* %this, %"class.Kokkos::RangePolicy"** %this.addr, align 8
  %this1 = load %"class.Kokkos::RangePolicy"*, %"class.Kokkos::RangePolicy"** %this.addr, align 8
  %call = call i32 @_ZN6Kokkos6OpenMP11concurrencyEv()
  %conv = sext i32 %call to i64
  store i64 %conv, i64* %concurrency, align 8
  %0 = load i64, i64* %concurrency, align 8
  %cmp = icmp eq i64 %0, 0
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  store i64 1, i64* %concurrency, align 8
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %m_granularity = getelementptr inbounds %"class.Kokkos::RangePolicy", %"class.Kokkos::RangePolicy"* %this1, i32 0, i32 3
  %1 = load i64, i64* %m_granularity, align 8
  %cmp2 = icmp ugt i64 %1, 0
  br i1 %cmp2, label %if.then3, label %if.end8

if.then3:                                         ; preds = %if.end
  %m_granularity4 = getelementptr inbounds %"class.Kokkos::RangePolicy", %"class.Kokkos::RangePolicy"* %this1, i32 0, i32 3
  %2 = load i64, i64* %m_granularity4, align 8
  %call5 = call zeroext i1 @_ZN6Kokkos4Impl24is_integral_power_of_twoEm(i64 %2)
  br i1 %call5, label %if.end7, label %if.then6

if.then6:                                         ; preds = %if.then3
  call void @_ZN6Kokkos5abortEPKc(i8* getelementptr inbounds ([54 x i8], [54 x i8]* @.str.6, i32 0, i32 0))
  br label %if.end7

if.end7:                                          ; preds = %if.then6, %if.then3
  br label %if.end8

if.end8:                                          ; preds = %if.end7, %if.end
  store i64 1, i64* %new_chunk_size, align 8
  br label %while.cond

while.cond:                                       ; preds = %while.body, %if.end8
  %3 = load i64, i64* %new_chunk_size, align 8
  %mul = mul i64 %3, 100
  %4 = load i64, i64* %concurrency, align 8
  %mul9 = mul i64 %mul, %4
  %m_end = getelementptr inbounds %"class.Kokkos::RangePolicy", %"class.Kokkos::RangePolicy"* %this1, i32 0, i32 2
  %5 = load i64, i64* %m_end, align 8
  %m_begin = getelementptr inbounds %"class.Kokkos::RangePolicy", %"class.Kokkos::RangePolicy"* %this1, i32 0, i32 1
  %6 = load i64, i64* %m_begin, align 8
  %sub = sub i64 %5, %6
  %cmp10 = icmp ult i64 %mul9, %sub
  br i1 %cmp10, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %7 = load i64, i64* %new_chunk_size, align 8
  %mul11 = mul i64 %7, 2
  store i64 %mul11, i64* %new_chunk_size, align 8
  br label %while.cond

while.end:                                        ; preds = %while.cond
  %8 = load i64, i64* %new_chunk_size, align 8
  %cmp12 = icmp ult i64 %8, 128
  br i1 %cmp12, label %if.then13, label %if.end25

if.then13:                                        ; preds = %while.end
  store i64 1, i64* %new_chunk_size, align 8
  br label %while.cond14

while.cond14:                                     ; preds = %while.body22, %if.then13
  %9 = load i64, i64* %new_chunk_size, align 8
  %mul15 = mul i64 %9, 40
  %10 = load i64, i64* %concurrency, align 8
  %mul16 = mul i64 %mul15, %10
  %m_end17 = getelementptr inbounds %"class.Kokkos::RangePolicy", %"class.Kokkos::RangePolicy"* %this1, i32 0, i32 2
  %11 = load i64, i64* %m_end17, align 8
  %m_begin18 = getelementptr inbounds %"class.Kokkos::RangePolicy", %"class.Kokkos::RangePolicy"* %this1, i32 0, i32 1
  %12 = load i64, i64* %m_begin18, align 8
  %sub19 = sub i64 %11, %12
  %cmp20 = icmp ult i64 %mul16, %sub19
  br i1 %cmp20, label %land.rhs, label %land.end

land.rhs:                                         ; preds = %while.cond14
  %13 = load i64, i64* %new_chunk_size, align 8
  %cmp21 = icmp ult i64 %13, 128
  br label %land.end

land.end:                                         ; preds = %land.rhs, %while.cond14
  %14 = phi i1 [ false, %while.cond14 ], [ %cmp21, %land.rhs ]
  br i1 %14, label %while.body22, label %while.end24

while.body22:                                     ; preds = %land.end
  %15 = load i64, i64* %new_chunk_size, align 8
  %mul23 = mul i64 %15, 2
  store i64 %mul23, i64* %new_chunk_size, align 8
  br label %while.cond14

while.end24:                                      ; preds = %land.end
  br label %if.end25

if.end25:                                         ; preds = %while.end24, %while.end
  %16 = load i64, i64* %new_chunk_size, align 8
  %m_granularity26 = getelementptr inbounds %"class.Kokkos::RangePolicy", %"class.Kokkos::RangePolicy"* %this1, i32 0, i32 3
  store i64 %16, i64* %m_granularity26, align 8
  %m_granularity27 = getelementptr inbounds %"class.Kokkos::RangePolicy", %"class.Kokkos::RangePolicy"* %this1, i32 0, i32 3
  %17 = load i64, i64* %m_granularity27, align 8
  %sub28 = sub i64 %17, 1
  %m_granularity_mask = getelementptr inbounds %"class.Kokkos::RangePolicy", %"class.Kokkos::RangePolicy"* %this1, i32 0, i32 4
  store i64 %sub28, i64* %m_granularity_mask, align 8
  ret void
}

declare i32 @_ZN6Kokkos6OpenMP11concurrencyEv() #1

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr zeroext i1 @_ZN6Kokkos4Impl24is_integral_power_of_twoEm(i64 %N) #4 comdat {
entry:
  %N.addr = alloca i64, align 8
  store i64 %N, i64* %N.addr, align 8
  %0 = load i64, i64* %N.addr, align 8
  %cmp = icmp ult i64 0, %0
  br i1 %cmp, label %land.rhs, label %land.end

land.rhs:                                         ; preds = %entry
  %1 = load i64, i64* %N.addr, align 8
  %2 = load i64, i64* %N.addr, align 8
  %sub = sub i64 %2, 1
  %and = and i64 %1, %sub
  %cmp1 = icmp eq i64 0, %and
  br label %land.end

land.end:                                         ; preds = %land.rhs, %entry
  %3 = phi i1 [ false, %entry ], [ %cmp1, %land.rhs ]
  ret i1 %3
}

; Function Attrs: noinline optnone uwtable
define linkonce_odr void @_ZN6Kokkos5abortEPKc(i8* %message) #2 comdat {
entry:
  %message.addr = alloca i8*, align 8
  store i8* %message, i8** %message.addr, align 8
  %0 = load i8*, i8** %message.addr, align 8
  call void @_ZN6Kokkos4Impl10host_abortEPKc(i8* %0)
  ret void
}

declare void @_ZN6Kokkos4Impl10host_abortEPKc(i8*) #1

; Function Attrs: noinline optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl11ParallelForINS0_16ViewValueFunctorINS_6OpenMPEdLb1EEENS_11RangePolicyIJS3_EEES3_E10exec_rangeIvEENSt9enable_ifIXsr3std7is_sameIT_vEE5valueEvE4typeERKS4_mm(%"struct.Kokkos::Impl::ViewValueFunctor"* dereferenceable(24) %functor, i64 %ibeg, i64 %iend) #2 comdat align 2 {
entry:
  %functor.addr = alloca %"struct.Kokkos::Impl::ViewValueFunctor"*, align 8
  %ibeg.addr = alloca i64, align 8
  %iend.addr = alloca i64, align 8
  %iwork = alloca i64, align 8
  store %"struct.Kokkos::Impl::ViewValueFunctor"* %functor, %"struct.Kokkos::Impl::ViewValueFunctor"** %functor.addr, align 8
  store i64 %ibeg, i64* %ibeg.addr, align 8
  store i64 %iend, i64* %iend.addr, align 8
  %0 = load i64, i64* %ibeg.addr, align 8
  store i64 %0, i64* %iwork, align 8
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %1 = load i64, i64* %iwork, align 8
  %2 = load i64, i64* %iend.addr, align 8
  %cmp = icmp ult i64 %1, %2
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %3 = load %"struct.Kokkos::Impl::ViewValueFunctor"*, %"struct.Kokkos::Impl::ViewValueFunctor"** %functor.addr, align 8
  %4 = load i64, i64* %iwork, align 8
  call void @_ZNK6Kokkos4Impl16ViewValueFunctorINS_6OpenMPEdLb1EEclEm(%"struct.Kokkos::Impl::ViewValueFunctor"* %3, i64 %4)
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %5 = load i64, i64* %iwork, align 8
  %inc = add i64 %5, 1
  store i64 %inc, i64* %iwork, align 8
  br label %for.cond

for.end:                                          ; preds = %for.cond
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr i64 @_ZNK6Kokkos11RangePolicyIJNS_6OpenMPEEE5beginEv(%"class.Kokkos::RangePolicy"* %this) #4 comdat align 2 {
entry:
  %this.addr = alloca %"class.Kokkos::RangePolicy"*, align 8
  store %"class.Kokkos::RangePolicy"* %this, %"class.Kokkos::RangePolicy"** %this.addr, align 8
  %this1 = load %"class.Kokkos::RangePolicy"*, %"class.Kokkos::RangePolicy"** %this.addr, align 8
  %m_begin = getelementptr inbounds %"class.Kokkos::RangePolicy", %"class.Kokkos::RangePolicy"* %this1, i32 0, i32 1
  %0 = load i64, i64* %m_begin, align 8
  ret i64 %0
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr i64 @_ZNK6Kokkos11RangePolicyIJNS_6OpenMPEEE3endEv(%"class.Kokkos::RangePolicy"* %this) #4 comdat align 2 {
entry:
  %this.addr = alloca %"class.Kokkos::RangePolicy"*, align 8
  store %"class.Kokkos::RangePolicy"* %this, %"class.Kokkos::RangePolicy"** %this.addr, align 8
  %this1 = load %"class.Kokkos::RangePolicy"*, %"class.Kokkos::RangePolicy"** %this.addr, align 8
  %m_end = getelementptr inbounds %"class.Kokkos::RangePolicy", %"class.Kokkos::RangePolicy"* %this1, i32 0, i32 2
  %0 = load i64, i64* %m_end, align 8
  ret i64 %0
}

declare void @_ZN6Kokkos4Impl10OpenMPExec16verify_is_masterEPKc(i8*) #1

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr i32 @_ZN6Kokkos6OpenMP16thread_pool_sizeEv() #4 comdat align 2 personality i8* bitcast (i32 (...)* @__gxx_personality_v0 to i8*) {
entry:
  %ref.tmp = alloca %"class.Kokkos::OpenMP", align 1
  call void @_ZN6Kokkos6OpenMPC2Ev(%"class.Kokkos::OpenMP"* %ref.tmp) #7
  %call = call zeroext i1 @_ZN6Kokkos6OpenMP11in_parallelERKS0_(%"class.Kokkos::OpenMP"* dereferenceable(1) %ref.tmp) #7
  br i1 %call, label %cond.true, label %cond.false

cond.true:                                        ; preds = %entry
  %call1 = invoke i32 @omp_get_num_threads()
          to label %invoke.cont unwind label %terminate.lpad

invoke.cont:                                      ; preds = %cond.true
  br label %cond.end

cond.false:                                       ; preds = %entry
  %0 = load %"class.Kokkos::Impl::OpenMPExec"*, %"class.Kokkos::Impl::OpenMPExec"** @_ZN6Kokkos4Impl17t_openmp_instanceE, align 8
  %m_pool_size = getelementptr inbounds %"class.Kokkos::Impl::OpenMPExec", %"class.Kokkos::Impl::OpenMPExec"* %0, i32 0, i32 0
  %1 = load i32, i32* %m_pool_size, align 8
  br label %cond.end

cond.end:                                         ; preds = %cond.false, %invoke.cont
  %cond = phi i32 [ %call1, %invoke.cont ], [ %1, %cond.false ]
  ret i32 %cond

terminate.lpad:                                   ; preds = %cond.true
  %2 = landingpad { i8*, i32 }
          catch i8* null
  %3 = extractvalue { i8*, i32 } %2, 0
  call void @__clang_call_terminate(i8* %3) #14
  unreachable
}

; Function Attrs: noinline nounwind optnone uwtable
define internal void @.omp_outlined.(i32* noalias %.global_tid., i32* noalias %.bound_tid., %"class.Kokkos::Impl::ParallelFor"* %this) #4 personality i8* bitcast (i32 (...)* @__gxx_personality_v0 to i8*) {
entry:
  %.global_tid..addr = alloca i32*, align 8
  %.bound_tid..addr = alloca i32*, align 8
  %this.addr = alloca %"class.Kokkos::Impl::ParallelFor"*, align 8
  %data = alloca %"class.Kokkos::Impl::HostThreadTeamData"*, align 8
  %range = alloca %"struct.std::pair", align 8
  %ref.tmp = alloca i32, align 4
  %ref.tmp8 = alloca i32, align 4
  %ref.tmp10 = alloca %"struct.std::pair", align 8
  store i32* %.global_tid., i32** %.global_tid..addr, align 8
  store i32* %.bound_tid., i32** %.bound_tid..addr, align 8
  store %"class.Kokkos::Impl::ParallelFor"* %this, %"class.Kokkos::Impl::ParallelFor"** %this.addr, align 8
  %0 = load %"class.Kokkos::Impl::ParallelFor"*, %"class.Kokkos::Impl::ParallelFor"** %this.addr, align 8
  %m_instance = getelementptr inbounds %"class.Kokkos::Impl::ParallelFor", %"class.Kokkos::Impl::ParallelFor"* %0, i32 0, i32 0
  %1 = load %"class.Kokkos::Impl::OpenMPExec"*, %"class.Kokkos::Impl::OpenMPExec"** %m_instance, align 8
  %call = call %"class.Kokkos::Impl::HostThreadTeamData"* @_ZNK6Kokkos4Impl10OpenMPExec15get_thread_dataEv(%"class.Kokkos::Impl::OpenMPExec"* %1) #7
  store %"class.Kokkos::Impl::HostThreadTeamData"* %call, %"class.Kokkos::Impl::HostThreadTeamData"** %data, align 8
  %2 = load %"class.Kokkos::Impl::HostThreadTeamData"*, %"class.Kokkos::Impl::HostThreadTeamData"** %data, align 8
  %m_policy = getelementptr inbounds %"class.Kokkos::Impl::ParallelFor", %"class.Kokkos::Impl::ParallelFor"* %0, i32 0, i32 2
  %call1 = invoke i64 @_ZNK6Kokkos11RangePolicyIJNS_6OpenMPEEE3endEv(%"class.Kokkos::RangePolicy"* %m_policy)
          to label %invoke.cont unwind label %terminate.lpad

invoke.cont:                                      ; preds = %entry
  %m_policy2 = getelementptr inbounds %"class.Kokkos::Impl::ParallelFor", %"class.Kokkos::Impl::ParallelFor"* %0, i32 0, i32 2
  %call4 = invoke i64 @_ZNK6Kokkos11RangePolicyIJNS_6OpenMPEEE5beginEv(%"class.Kokkos::RangePolicy"* %m_policy2)
          to label %invoke.cont3 unwind label %terminate.lpad

invoke.cont3:                                     ; preds = %invoke.cont
  %sub = sub i64 %call1, %call4
  %m_policy5 = getelementptr inbounds %"class.Kokkos::Impl::ParallelFor", %"class.Kokkos::Impl::ParallelFor"* %0, i32 0, i32 2
  %call7 = invoke i64 @_ZNK6Kokkos11RangePolicyIJNS_6OpenMPEEE10chunk_sizeEv(%"class.Kokkos::RangePolicy"* %m_policy5)
          to label %invoke.cont6 unwind label %terminate.lpad

invoke.cont6:                                     ; preds = %invoke.cont3
  %conv = trunc i64 %call7 to i32
  call void @_ZN6Kokkos4Impl18HostThreadTeamData18set_work_partitionEli(%"class.Kokkos::Impl::HostThreadTeamData"* %2, i64 %sub, i32 %conv) #7
  store i32 0, i32* %ref.tmp, align 4
  store i32 0, i32* %ref.tmp8, align 4
  invoke void @_ZNSt4pairIllEC2IiiLb1EEEOT_OT0_(%"struct.std::pair"* %range, i32* dereferenceable(4) %ref.tmp, i32* dereferenceable(4) %ref.tmp8)
          to label %invoke.cont9 unwind label %terminate.lpad

invoke.cont9:                                     ; preds = %invoke.cont6
  br label %do.body

do.body:                                          ; preds = %invoke.cont9
  br i1 false, label %cond.true, label %cond.false

cond.true:                                        ; preds = %do.body
  %3 = load %"class.Kokkos::Impl::HostThreadTeamData"*, %"class.Kokkos::Impl::HostThreadTeamData"** %data, align 8
  %call11 = call { i64, i64 } @_ZN6Kokkos4Impl18HostThreadTeamData23get_work_stealing_chunkEv(%"class.Kokkos::Impl::HostThreadTeamData"* %3) #7
  %4 = bitcast %"struct.std::pair"* %ref.tmp10 to { i64, i64 }*
  %5 = getelementptr inbounds { i64, i64 }, { i64, i64 }* %4, i32 0, i32 0
  %6 = extractvalue { i64, i64 } %call11, 0
  store i64 %6, i64* %5, align 8
  %7 = getelementptr inbounds { i64, i64 }, { i64, i64 }* %4, i32 0, i32 1
  %8 = extractvalue { i64, i64 } %call11, 1
  store i64 %8, i64* %7, align 8
  br label %cond.end

cond.false:                                       ; preds = %do.body
  %9 = load %"class.Kokkos::Impl::HostThreadTeamData"*, %"class.Kokkos::Impl::HostThreadTeamData"** %data, align 8
  %call12 = call { i64, i64 } @_ZN6Kokkos4Impl18HostThreadTeamData18get_work_partitionEv(%"class.Kokkos::Impl::HostThreadTeamData"* %9) #7
  %10 = bitcast %"struct.std::pair"* %ref.tmp10 to { i64, i64 }*
  %11 = getelementptr inbounds { i64, i64 }, { i64, i64 }* %10, i32 0, i32 0
  %12 = extractvalue { i64, i64 } %call12, 0
  store i64 %12, i64* %11, align 8
  %13 = getelementptr inbounds { i64, i64 }, { i64, i64 }* %10, i32 0, i32 1
  %14 = extractvalue { i64, i64 } %call12, 1
  store i64 %14, i64* %13, align 8
  br label %cond.end

cond.end:                                         ; preds = %cond.false, %cond.true
  %call13 = call dereferenceable(16) %"struct.std::pair"* @_ZNSt4pairIllEaSEOS0_(%"struct.std::pair"* %range, %"struct.std::pair"* dereferenceable(16) %ref.tmp10) #7
  %m_functor = getelementptr inbounds %"class.Kokkos::Impl::ParallelFor", %"class.Kokkos::Impl::ParallelFor"* %0, i32 0, i32 1
  %first = getelementptr inbounds %"struct.std::pair", %"struct.std::pair"* %range, i32 0, i32 0
  %15 = load i64, i64* %first, align 8
  %m_policy14 = getelementptr inbounds %"class.Kokkos::Impl::ParallelFor", %"class.Kokkos::Impl::ParallelFor"* %0, i32 0, i32 2
  %call16 = invoke i64 @_ZNK6Kokkos11RangePolicyIJNS_6OpenMPEEE5beginEv(%"class.Kokkos::RangePolicy"* %m_policy14)
          to label %invoke.cont15 unwind label %terminate.lpad

invoke.cont15:                                    ; preds = %cond.end
  %add = add i64 %15, %call16
  %second = getelementptr inbounds %"struct.std::pair", %"struct.std::pair"* %range, i32 0, i32 1
  %16 = load i64, i64* %second, align 8
  %m_policy17 = getelementptr inbounds %"class.Kokkos::Impl::ParallelFor", %"class.Kokkos::Impl::ParallelFor"* %0, i32 0, i32 2
  %call19 = invoke i64 @_ZNK6Kokkos11RangePolicyIJNS_6OpenMPEEE5beginEv(%"class.Kokkos::RangePolicy"* %m_policy17)
          to label %invoke.cont18 unwind label %terminate.lpad

invoke.cont18:                                    ; preds = %invoke.cont15
  %add20 = add i64 %16, %call19
  invoke void @_ZN6Kokkos4Impl11ParallelForINS0_16ViewValueFunctorINS_6OpenMPEdLb1EEENS_11RangePolicyIJS3_EEES3_E10exec_rangeIvEENSt9enable_ifIXsr3std7is_sameIT_vEE5valueEvE4typeERKS4_mm(%"struct.Kokkos::Impl::ViewValueFunctor"* dereferenceable(24) %m_functor, i64 %add, i64 %add20)
          to label %invoke.cont21 unwind label %terminate.lpad

invoke.cont21:                                    ; preds = %invoke.cont18
  br label %do.cond

do.cond:                                          ; preds = %invoke.cont21
  br label %do.end

do.end:                                           ; preds = %do.cond
  ret void

terminate.lpad:                                   ; preds = %invoke.cont18, %invoke.cont15, %cond.end, %invoke.cont6, %invoke.cont3, %invoke.cont, %entry
  %17 = landingpad { i8*, i32 }
          catch i8* null
  %18 = extractvalue { i8*, i32 } %17, 0
  call void @__clang_call_terminate(i8* %18) #14
  unreachable
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr %"class.Kokkos::Impl::HostThreadTeamData"* @_ZNK6Kokkos4Impl10OpenMPExec15get_thread_dataEv(%"class.Kokkos::Impl::OpenMPExec"* %this) #4 comdat align 2 personality i8* bitcast (i32 (...)* @__gxx_personality_v0 to i8*) {
entry:
  %this.addr = alloca %"class.Kokkos::Impl::OpenMPExec"*, align 8
  store %"class.Kokkos::Impl::OpenMPExec"* %this, %"class.Kokkos::Impl::OpenMPExec"** %this.addr, align 8
  %this1 = load %"class.Kokkos::Impl::OpenMPExec"*, %"class.Kokkos::Impl::OpenMPExec"** %this.addr, align 8
  %m_pool = getelementptr inbounds %"class.Kokkos::Impl::OpenMPExec", %"class.Kokkos::Impl::OpenMPExec"* %this1, i32 0, i32 2
  %m_level = getelementptr inbounds %"class.Kokkos::Impl::OpenMPExec", %"class.Kokkos::Impl::OpenMPExec"* %this1, i32 0, i32 1
  %0 = load i32, i32* %m_level, align 4
  %call = invoke i32 @omp_get_level()
          to label %invoke.cont unwind label %terminate.lpad

invoke.cont:                                      ; preds = %entry
  %cmp = icmp eq i32 %0, %call
  br i1 %cmp, label %cond.true, label %cond.false

cond.true:                                        ; preds = %invoke.cont
  br label %cond.end

cond.false:                                       ; preds = %invoke.cont
  %call3 = invoke i32 @omp_get_thread_num()
          to label %invoke.cont2 unwind label %terminate.lpad

invoke.cont2:                                     ; preds = %cond.false
  br label %cond.end

cond.end:                                         ; preds = %invoke.cont2, %cond.true
  %cond = phi i32 [ 0, %cond.true ], [ %call3, %invoke.cont2 ]
  %idxprom = sext i32 %cond to i64
  %arrayidx = getelementptr inbounds [512 x %"class.Kokkos::Impl::HostThreadTeamData"*], [512 x %"class.Kokkos::Impl::HostThreadTeamData"*]* %m_pool, i64 0, i64 %idxprom
  %1 = load %"class.Kokkos::Impl::HostThreadTeamData"*, %"class.Kokkos::Impl::HostThreadTeamData"** %arrayidx, align 8
  ret %"class.Kokkos::Impl::HostThreadTeamData"* %1

terminate.lpad:                                   ; preds = %cond.false, %entry
  %2 = landingpad { i8*, i32 }
          catch i8* null
  %3 = extractvalue { i8*, i32 } %2, 0
  call void @__clang_call_terminate(i8* %3) #14
  unreachable
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr void @_ZN6Kokkos4Impl18HostThreadTeamData18set_work_partitionEli(%"class.Kokkos::Impl::HostThreadTeamData"* %this, i64 %length, i32 %chunk) #4 comdat align 2 personality i8* bitcast (i32 (...)* @__gxx_personality_v0 to i8*) {
entry:
  %this.addr = alloca %"class.Kokkos::Impl::HostThreadTeamData"*, align 8
  %length.addr = alloca i64, align 8
  %chunk.addr = alloca i32, align 4
  %chunk_min = alloca i32, align 4
  %num = alloca i32, align 4
  %part = alloca i32, align 4
  store %"class.Kokkos::Impl::HostThreadTeamData"* %this, %"class.Kokkos::Impl::HostThreadTeamData"** %this.addr, align 8
  store i64 %length, i64* %length.addr, align 8
  store i32 %chunk, i32* %chunk.addr, align 4
  %this1 = load %"class.Kokkos::Impl::HostThreadTeamData"*, %"class.Kokkos::Impl::HostThreadTeamData"** %this.addr, align 8
  %0 = load i64, i64* %length.addr, align 8
  %call = call i32 @_ZNSt14numeric_limitsIiE3maxEv() #7
  %conv = sext i32 %call to i64
  %add = add nsw i64 %0, %conv
  %call2 = call i32 @_ZNSt14numeric_limitsIiE3maxEv() #7
  %conv3 = sext i32 %call2 to i64
  %div = sdiv i64 %add, %conv3
  %conv4 = trunc i64 %div to i32
  store i32 %conv4, i32* %chunk_min, align 4
  %1 = load i64, i64* %length.addr, align 8
  %m_work_end = getelementptr inbounds %"class.Kokkos::Impl::HostThreadTeamData", %"class.Kokkos::Impl::HostThreadTeamData"* %this1, i32 0, i32 1
  store i64 %1, i64* %m_work_end, align 8
  %call5 = invoke dereferenceable(4) i32* @_ZSt3maxIiERKT_S2_S2_(i32* dereferenceable(4) %chunk.addr, i32* dereferenceable(4) %chunk_min)
          to label %invoke.cont unwind label %terminate.lpad

invoke.cont:                                      ; preds = %entry
  %2 = load i32, i32* %call5, align 4
  %m_work_chunk = getelementptr inbounds %"class.Kokkos::Impl::HostThreadTeamData", %"class.Kokkos::Impl::HostThreadTeamData"* %this1, i32 0, i32 17
  store i32 %2, i32* %m_work_chunk, align 8
  %m_work_end6 = getelementptr inbounds %"class.Kokkos::Impl::HostThreadTeamData", %"class.Kokkos::Impl::HostThreadTeamData"* %this1, i32 0, i32 1
  %3 = load i64, i64* %m_work_end6, align 8
  %m_work_chunk7 = getelementptr inbounds %"class.Kokkos::Impl::HostThreadTeamData", %"class.Kokkos::Impl::HostThreadTeamData"* %this1, i32 0, i32 17
  %4 = load i32, i32* %m_work_chunk7, align 8
  %conv8 = sext i32 %4 to i64
  %add9 = add nsw i64 %3, %conv8
  %sub = sub nsw i64 %add9, 1
  %m_work_chunk10 = getelementptr inbounds %"class.Kokkos::Impl::HostThreadTeamData", %"class.Kokkos::Impl::HostThreadTeamData"* %this1, i32 0, i32 17
  %5 = load i32, i32* %m_work_chunk10, align 8
  %conv11 = sext i32 %5 to i64
  %div12 = sdiv i64 %sub, %conv11
  %conv13 = trunc i64 %div12 to i32
  store i32 %conv13, i32* %num, align 4
  %6 = load i32, i32* %num, align 4
  %m_league_size = getelementptr inbounds %"class.Kokkos::Impl::HostThreadTeamData", %"class.Kokkos::Impl::HostThreadTeamData"* %this1, i32 0, i32 16
  %7 = load i32, i32* %m_league_size, align 4
  %add14 = add nsw i32 %6, %7
  %sub15 = sub nsw i32 %add14, 1
  %m_league_size16 = getelementptr inbounds %"class.Kokkos::Impl::HostThreadTeamData", %"class.Kokkos::Impl::HostThreadTeamData"* %this1, i32 0, i32 16
  %8 = load i32, i32* %m_league_size16, align 4
  %div17 = sdiv i32 %sub15, %8
  store i32 %div17, i32* %part, align 4
  %9 = load i32, i32* %part, align 4
  %m_league_rank = getelementptr inbounds %"class.Kokkos::Impl::HostThreadTeamData", %"class.Kokkos::Impl::HostThreadTeamData"* %this1, i32 0, i32 15
  %10 = load i32, i32* %m_league_rank, align 8
  %mul = mul nsw i32 %9, %10
  %conv18 = sext i32 %mul to i64
  %m_work_range = getelementptr inbounds %"class.Kokkos::Impl::HostThreadTeamData", %"class.Kokkos::Impl::HostThreadTeamData"* %this1, i32 0, i32 0
  %first = getelementptr inbounds %"struct.Kokkos::pair", %"struct.Kokkos::pair"* %m_work_range, i32 0, i32 0
  store i64 %conv18, i64* %first, align 8
  %m_work_range19 = getelementptr inbounds %"class.Kokkos::Impl::HostThreadTeamData", %"class.Kokkos::Impl::HostThreadTeamData"* %this1, i32 0, i32 0
  %first20 = getelementptr inbounds %"struct.Kokkos::pair", %"struct.Kokkos::pair"* %m_work_range19, i32 0, i32 0
  %11 = load i64, i64* %first20, align 8
  %12 = load i32, i32* %part, align 4
  %conv21 = sext i32 %12 to i64
  %add22 = add nsw i64 %11, %conv21
  %m_work_range23 = getelementptr inbounds %"class.Kokkos::Impl::HostThreadTeamData", %"class.Kokkos::Impl::HostThreadTeamData"* %this1, i32 0, i32 0
  %second = getelementptr inbounds %"struct.Kokkos::pair", %"struct.Kokkos::pair"* %m_work_range23, i32 0, i32 1
  store i64 %add22, i64* %second, align 8
  %m_team_base = getelementptr inbounds %"class.Kokkos::Impl::HostThreadTeamData", %"class.Kokkos::Impl::HostThreadTeamData"* %this1, i32 0, i32 11
  %13 = load i32, i32* %m_team_base, align 8
  %m_team_alloc = getelementptr inbounds %"class.Kokkos::Impl::HostThreadTeamData", %"class.Kokkos::Impl::HostThreadTeamData"* %this1, i32 0, i32 14
  %14 = load i32, i32* %m_team_alloc, align 4
  %add24 = add nsw i32 %13, %14
  %m_team_size = getelementptr inbounds %"class.Kokkos::Impl::HostThreadTeamData", %"class.Kokkos::Impl::HostThreadTeamData"* %this1, i32 0, i32 13
  %15 = load i32, i32* %m_team_size, align 8
  %add25 = add nsw i32 %add24, %15
  %m_pool_size = getelementptr inbounds %"class.Kokkos::Impl::HostThreadTeamData", %"class.Kokkos::Impl::HostThreadTeamData"* %this1, i32 0, i32 6
  %16 = load i32, i32* %m_pool_size, align 4
  %cmp = icmp sle i32 %add25, %16
  br i1 %cmp, label %cond.true, label %cond.false

cond.true:                                        ; preds = %invoke.cont
  %m_team_base26 = getelementptr inbounds %"class.Kokkos::Impl::HostThreadTeamData", %"class.Kokkos::Impl::HostThreadTeamData"* %this1, i32 0, i32 11
  %17 = load i32, i32* %m_team_base26, align 8
  %m_team_alloc27 = getelementptr inbounds %"class.Kokkos::Impl::HostThreadTeamData", %"class.Kokkos::Impl::HostThreadTeamData"* %this1, i32 0, i32 14
  %18 = load i32, i32* %m_team_alloc27, align 4
  %add28 = add nsw i32 %17, %18
  br label %cond.end

cond.false:                                       ; preds = %invoke.cont
  br label %cond.end

cond.end:                                         ; preds = %cond.false, %cond.true
  %cond = phi i32 [ %add28, %cond.true ], [ 0, %cond.false ]
  %m_steal_rank = getelementptr inbounds %"class.Kokkos::Impl::HostThreadTeamData", %"class.Kokkos::Impl::HostThreadTeamData"* %this1, i32 0, i32 18
  store i32 %cond, i32* %m_steal_rank, align 4
  ret void

terminate.lpad:                                   ; preds = %entry
  %19 = landingpad { i8*, i32 }
          catch i8* null
  %20 = extractvalue { i8*, i32 } %19, 0
  call void @__clang_call_terminate(i8* %20) #14
  unreachable
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr i64 @_ZNK6Kokkos11RangePolicyIJNS_6OpenMPEEE10chunk_sizeEv(%"class.Kokkos::RangePolicy"* %this) #4 comdat align 2 {
entry:
  %this.addr = alloca %"class.Kokkos::RangePolicy"*, align 8
  store %"class.Kokkos::RangePolicy"* %this, %"class.Kokkos::RangePolicy"** %this.addr, align 8
  %this1 = load %"class.Kokkos::RangePolicy"*, %"class.Kokkos::RangePolicy"** %this.addr, align 8
  %m_granularity = getelementptr inbounds %"class.Kokkos::RangePolicy", %"class.Kokkos::RangePolicy"* %this1, i32 0, i32 3
  %0 = load i64, i64* %m_granularity, align 8
  ret i64 %0
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr void @_ZNSt4pairIllEC2IiiLb1EEEOT_OT0_(%"struct.std::pair"* %this, i32* dereferenceable(4) %__x, i32* dereferenceable(4) %__y) unnamed_addr #4 comdat align 2 {
entry:
  %this.addr = alloca %"struct.std::pair"*, align 8
  %__x.addr = alloca i32*, align 8
  %__y.addr = alloca i32*, align 8
  store %"struct.std::pair"* %this, %"struct.std::pair"** %this.addr, align 8
  store i32* %__x, i32** %__x.addr, align 8
  store i32* %__y, i32** %__y.addr, align 8
  %this1 = load %"struct.std::pair"*, %"struct.std::pair"** %this.addr, align 8
  %first = getelementptr inbounds %"struct.std::pair", %"struct.std::pair"* %this1, i32 0, i32 0
  %0 = load i32*, i32** %__x.addr, align 8
  %call = call dereferenceable(4) i32* @_ZSt7forwardIiEOT_RNSt16remove_referenceIS0_E4typeE(i32* dereferenceable(4) %0) #7
  %1 = load i32, i32* %call, align 4
  %conv = sext i32 %1 to i64
  store i64 %conv, i64* %first, align 8
  %second = getelementptr inbounds %"struct.std::pair", %"struct.std::pair"* %this1, i32 0, i32 1
  %2 = load i32*, i32** %__y.addr, align 8
  %call2 = call dereferenceable(4) i32* @_ZSt7forwardIiEOT_RNSt16remove_referenceIS0_E4typeE(i32* dereferenceable(4) %2) #7
  %3 = load i32, i32* %call2, align 4
  %conv3 = sext i32 %3 to i64
  store i64 %conv3, i64* %second, align 8
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr { i64, i64 } @_ZN6Kokkos4Impl18HostThreadTeamData23get_work_stealing_chunkEv(%"class.Kokkos::Impl::HostThreadTeamData"* %this) #4 comdat align 2 personality i8* bitcast (i32 (...)* @__gxx_personality_v0 to i8*) {
entry:
  %retval = alloca %"struct.std::pair", align 8
  %this.addr = alloca %"class.Kokkos::Impl::HostThreadTeamData"*, align 8
  %ref.tmp = alloca i32, align 4
  %ref.tmp2 = alloca i32, align 4
  %i = alloca i32, align 4
  store %"class.Kokkos::Impl::HostThreadTeamData"* %this, %"class.Kokkos::Impl::HostThreadTeamData"** %this.addr, align 8
  %this1 = load %"class.Kokkos::Impl::HostThreadTeamData"*, %"class.Kokkos::Impl::HostThreadTeamData"** %this.addr, align 8
  store i32 -1, i32* %ref.tmp, align 4
  store i32 -1, i32* %ref.tmp2, align 4
  invoke void @_ZNSt4pairIllEC2IiiLb1EEEOT_OT0_(%"struct.std::pair"* %retval, i32* dereferenceable(4) %ref.tmp, i32* dereferenceable(4) %ref.tmp2)
          to label %invoke.cont unwind label %terminate.lpad

invoke.cont:                                      ; preds = %entry
  %call = call i32 @_ZN6Kokkos4Impl18HostThreadTeamData17get_work_stealingEv(%"class.Kokkos::Impl::HostThreadTeamData"* %this1) #7
  store i32 %call, i32* %i, align 4
  %0 = load i32, i32* %i, align 4
  %cmp = icmp sle i32 0, %0
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %invoke.cont
  %m_work_chunk = getelementptr inbounds %"class.Kokkos::Impl::HostThreadTeamData", %"class.Kokkos::Impl::HostThreadTeamData"* %this1, i32 0, i32 17
  %1 = load i32, i32* %m_work_chunk, align 8
  %2 = load i32, i32* %i, align 4
  %mul = mul nsw i32 %1, %2
  %conv = sext i32 %mul to i64
  %first = getelementptr inbounds %"struct.std::pair", %"struct.std::pair"* %retval, i32 0, i32 0
  store i64 %conv, i64* %first, align 8
  %first3 = getelementptr inbounds %"struct.std::pair", %"struct.std::pair"* %retval, i32 0, i32 0
  %3 = load i64, i64* %first3, align 8
  %m_work_chunk4 = getelementptr inbounds %"class.Kokkos::Impl::HostThreadTeamData", %"class.Kokkos::Impl::HostThreadTeamData"* %this1, i32 0, i32 17
  %4 = load i32, i32* %m_work_chunk4, align 8
  %conv5 = sext i32 %4 to i64
  %add = add nsw i64 %3, %conv5
  %m_work_end = getelementptr inbounds %"class.Kokkos::Impl::HostThreadTeamData", %"class.Kokkos::Impl::HostThreadTeamData"* %this1, i32 0, i32 1
  %5 = load i64, i64* %m_work_end, align 8
  %cmp6 = icmp slt i64 %add, %5
  br i1 %cmp6, label %cond.true, label %cond.false

cond.true:                                        ; preds = %if.then
  %first7 = getelementptr inbounds %"struct.std::pair", %"struct.std::pair"* %retval, i32 0, i32 0
  %6 = load i64, i64* %first7, align 8
  %m_work_chunk8 = getelementptr inbounds %"class.Kokkos::Impl::HostThreadTeamData", %"class.Kokkos::Impl::HostThreadTeamData"* %this1, i32 0, i32 17
  %7 = load i32, i32* %m_work_chunk8, align 8
  %conv9 = sext i32 %7 to i64
  %add10 = add nsw i64 %6, %conv9
  br label %cond.end

cond.false:                                       ; preds = %if.then
  %m_work_end11 = getelementptr inbounds %"class.Kokkos::Impl::HostThreadTeamData", %"class.Kokkos::Impl::HostThreadTeamData"* %this1, i32 0, i32 1
  %8 = load i64, i64* %m_work_end11, align 8
  br label %cond.end

cond.end:                                         ; preds = %cond.false, %cond.true
  %cond = phi i64 [ %add10, %cond.true ], [ %8, %cond.false ]
  %second = getelementptr inbounds %"struct.std::pair", %"struct.std::pair"* %retval, i32 0, i32 1
  store i64 %cond, i64* %second, align 8
  br label %if.end

if.end:                                           ; preds = %cond.end, %invoke.cont
  %9 = bitcast %"struct.std::pair"* %retval to { i64, i64 }*
  %10 = load { i64, i64 }, { i64, i64 }* %9, align 8
  ret { i64, i64 } %10

terminate.lpad:                                   ; preds = %entry
  %11 = landingpad { i8*, i32 }
          catch i8* null
  %12 = extractvalue { i8*, i32 } %11, 0
  call void @__clang_call_terminate(i8* %12) #14
  unreachable
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr { i64, i64 } @_ZN6Kokkos4Impl18HostThreadTeamData18get_work_partitionEv(%"class.Kokkos::Impl::HostThreadTeamData"* %this) #4 comdat align 2 personality i8* bitcast (i32 (...)* @__gxx_personality_v0 to i8*) {
entry:
  %retval = alloca %"struct.std::pair", align 8
  %this.addr = alloca %"class.Kokkos::Impl::HostThreadTeamData"*, align 8
  %first = alloca i64, align 8
  %second = alloca i64, align 8
  store %"class.Kokkos::Impl::HostThreadTeamData"* %this, %"class.Kokkos::Impl::HostThreadTeamData"** %this.addr, align 8
  %this1 = load %"class.Kokkos::Impl::HostThreadTeamData"*, %"class.Kokkos::Impl::HostThreadTeamData"** %this.addr, align 8
  %m_work_range = getelementptr inbounds %"class.Kokkos::Impl::HostThreadTeamData", %"class.Kokkos::Impl::HostThreadTeamData"* %this1, i32 0, i32 0
  %first2 = getelementptr inbounds %"struct.Kokkos::pair", %"struct.Kokkos::pair"* %m_work_range, i32 0, i32 0
  %0 = load i64, i64* %first2, align 8
  store i64 %0, i64* %first, align 8
  %m_work_range3 = getelementptr inbounds %"class.Kokkos::Impl::HostThreadTeamData", %"class.Kokkos::Impl::HostThreadTeamData"* %this1, i32 0, i32 0
  %second4 = getelementptr inbounds %"struct.Kokkos::pair", %"struct.Kokkos::pair"* %m_work_range3, i32 0, i32 1
  %1 = load i64, i64* %second4, align 8
  store i64 %1, i64* %second, align 8
  %m_work_chunk = getelementptr inbounds %"class.Kokkos::Impl::HostThreadTeamData", %"class.Kokkos::Impl::HostThreadTeamData"* %this1, i32 0, i32 17
  %2 = load i32, i32* %m_work_chunk, align 8
  %conv = sext i32 %2 to i64
  %3 = load i64, i64* %first, align 8
  %mul = mul nsw i64 %3, %conv
  store i64 %mul, i64* %first, align 8
  %m_work_chunk5 = getelementptr inbounds %"class.Kokkos::Impl::HostThreadTeamData", %"class.Kokkos::Impl::HostThreadTeamData"* %this1, i32 0, i32 17
  %4 = load i32, i32* %m_work_chunk5, align 8
  %conv6 = sext i32 %4 to i64
  %5 = load i64, i64* %second, align 8
  %mul7 = mul nsw i64 %5, %conv6
  store i64 %mul7, i64* %second, align 8
  %6 = load i64, i64* %second, align 8
  %m_work_end = getelementptr inbounds %"class.Kokkos::Impl::HostThreadTeamData", %"class.Kokkos::Impl::HostThreadTeamData"* %this1, i32 0, i32 1
  %7 = load i64, i64* %m_work_end, align 8
  %cmp = icmp slt i64 %6, %7
  br i1 %cmp, label %cond.true, label %cond.false

cond.true:                                        ; preds = %entry
  br label %cond.end

cond.false:                                       ; preds = %entry
  %m_work_end8 = getelementptr inbounds %"class.Kokkos::Impl::HostThreadTeamData", %"class.Kokkos::Impl::HostThreadTeamData"* %this1, i32 0, i32 1
  br label %cond.end

cond.end:                                         ; preds = %cond.false, %cond.true
  %cond-lvalue = phi i64* [ %second, %cond.true ], [ %m_work_end8, %cond.false ]
  invoke void @_ZNSt4pairIllEC2IRlS2_Lb1EEEOT_OT0_(%"struct.std::pair"* %retval, i64* dereferenceable(8) %first, i64* dereferenceable(8) %cond-lvalue)
          to label %invoke.cont unwind label %terminate.lpad

invoke.cont:                                      ; preds = %cond.end
  %8 = bitcast %"struct.std::pair"* %retval to { i64, i64 }*
  %9 = load { i64, i64 }, { i64, i64 }* %8, align 8
  ret { i64, i64 } %9

terminate.lpad:                                   ; preds = %cond.end
  %10 = landingpad { i8*, i32 }
          catch i8* null
  %11 = extractvalue { i8*, i32 } %10, 0
  call void @__clang_call_terminate(i8* %11) #14
  unreachable
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dereferenceable(16) %"struct.std::pair"* @_ZNSt4pairIllEaSEOS0_(%"struct.std::pair"* %this, %"struct.std::pair"* dereferenceable(16) %__p) #4 comdat align 2 {
entry:
  %this.addr = alloca %"struct.std::pair"*, align 8
  %__p.addr = alloca %"struct.std::pair"*, align 8
  store %"struct.std::pair"* %this, %"struct.std::pair"** %this.addr, align 8
  store %"struct.std::pair"* %__p, %"struct.std::pair"** %__p.addr, align 8
  %this1 = load %"struct.std::pair"*, %"struct.std::pair"** %this.addr, align 8
  %0 = load %"struct.std::pair"*, %"struct.std::pair"** %__p.addr, align 8
  %first = getelementptr inbounds %"struct.std::pair", %"struct.std::pair"* %0, i32 0, i32 0
  %call = call dereferenceable(8) i64* @_ZSt7forwardIlEOT_RNSt16remove_referenceIS0_E4typeE(i64* dereferenceable(8) %first) #7
  %1 = load i64, i64* %call, align 8
  %first2 = getelementptr inbounds %"struct.std::pair", %"struct.std::pair"* %this1, i32 0, i32 0
  store i64 %1, i64* %first2, align 8
  %2 = load %"struct.std::pair"*, %"struct.std::pair"** %__p.addr, align 8
  %second = getelementptr inbounds %"struct.std::pair", %"struct.std::pair"* %2, i32 0, i32 1
  %call3 = call dereferenceable(8) i64* @_ZSt7forwardIlEOT_RNSt16remove_referenceIS0_E4typeE(i64* dereferenceable(8) %second) #7
  %3 = load i64, i64* %call3, align 8
  %second4 = getelementptr inbounds %"struct.std::pair", %"struct.std::pair"* %this1, i32 0, i32 1
  store i64 %3, i64* %second4, align 8
  ret %"struct.std::pair"* %this1
}

declare i32 @__kmpc_global_thread_num(%ident_t*)

declare void @__kmpc_push_num_threads(%ident_t*, i32, i32)

declare void @__kmpc_fork_call(%ident_t*, i32, void (i32*, i32*, ...)*, ...)

declare i32 @omp_get_num_threads() #1

declare i32 @omp_get_thread_num() #1

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr i32 @_ZNSt14numeric_limitsIiE3maxEv() #4 comdat align 2 {
entry:
  ret i32 2147483647
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dereferenceable(4) i32* @_ZSt3maxIiERKT_S2_S2_(i32* dereferenceable(4) %__a, i32* dereferenceable(4) %__b) #4 comdat {
entry:
  %retval = alloca i32*, align 8
  %__a.addr = alloca i32*, align 8
  %__b.addr = alloca i32*, align 8
  store i32* %__a, i32** %__a.addr, align 8
  store i32* %__b, i32** %__b.addr, align 8
  %0 = load i32*, i32** %__a.addr, align 8
  %1 = load i32, i32* %0, align 4
  %2 = load i32*, i32** %__b.addr, align 8
  %3 = load i32, i32* %2, align 4
  %cmp = icmp slt i32 %1, %3
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %4 = load i32*, i32** %__b.addr, align 8
  store i32* %4, i32** %retval, align 8
  br label %return

if.end:                                           ; preds = %entry
  %5 = load i32*, i32** %__a.addr, align 8
  store i32* %5, i32** %retval, align 8
  br label %return

return:                                           ; preds = %if.end, %if.then
  %6 = load i32*, i32** %retval, align 8
  ret i32* %6
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dereferenceable(4) i32* @_ZSt7forwardIiEOT_RNSt16remove_referenceIS0_E4typeE(i32* dereferenceable(4) %__t) #4 comdat {
entry:
  %__t.addr = alloca i32*, align 8
  store i32* %__t, i32** %__t.addr, align 8
  %0 = load i32*, i32** %__t.addr, align 8
  ret i32* %0
}

; Function Attrs: nounwind
declare i32 @_ZN6Kokkos4Impl18HostThreadTeamData17get_work_stealingEv(%"class.Kokkos::Impl::HostThreadTeamData"*) #6

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr void @_ZNSt4pairIllEC2IRlS2_Lb1EEEOT_OT0_(%"struct.std::pair"* %this, i64* dereferenceable(8) %__x, i64* dereferenceable(8) %__y) unnamed_addr #4 comdat align 2 {
entry:
  %this.addr = alloca %"struct.std::pair"*, align 8
  %__x.addr = alloca i64*, align 8
  %__y.addr = alloca i64*, align 8
  store %"struct.std::pair"* %this, %"struct.std::pair"** %this.addr, align 8
  store i64* %__x, i64** %__x.addr, align 8
  store i64* %__y, i64** %__y.addr, align 8
  %this1 = load %"struct.std::pair"*, %"struct.std::pair"** %this.addr, align 8
  %first = getelementptr inbounds %"struct.std::pair", %"struct.std::pair"* %this1, i32 0, i32 0
  %0 = load i64*, i64** %__x.addr, align 8
  %call = call dereferenceable(8) i64* @_ZSt7forwardIRlEOT_RNSt16remove_referenceIS1_E4typeE(i64* dereferenceable(8) %0) #7
  %1 = load i64, i64* %call, align 8
  store i64 %1, i64* %first, align 8
  %second = getelementptr inbounds %"struct.std::pair", %"struct.std::pair"* %this1, i32 0, i32 1
  %2 = load i64*, i64** %__y.addr, align 8
  %call2 = call dereferenceable(8) i64* @_ZSt7forwardIRlEOT_RNSt16remove_referenceIS1_E4typeE(i64* dereferenceable(8) %2) #7
  %3 = load i64, i64* %call2, align 8
  store i64 %3, i64* %second, align 8
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dereferenceable(8) i64* @_ZSt7forwardIRlEOT_RNSt16remove_referenceIS1_E4typeE(i64* dereferenceable(8) %__t) #4 comdat {
entry:
  %__t.addr = alloca i64*, align 8
  store i64* %__t, i64** %__t.addr, align 8
  %0 = load i64*, i64** %__t.addr, align 8
  ret i64* %0
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr dereferenceable(8) i64* @_ZSt7forwardIlEOT_RNSt16remove_referenceIS0_E4typeE(i64* dereferenceable(8) %__t) #4 comdat {
entry:
  %__t.addr = alloca i64*, align 8
  store i64* %__t, i64** %__t.addr, align 8
  %0 = load i64*, i64** %__t.addr, align 8
  ret i64* %0
}

declare void @_ZN6Kokkos4Impl22SharedAllocationRecordIvvE9incrementEPS2_(%"class.Kokkos::Impl::SharedAllocationRecord"*) #1

declare %"class.Kokkos::Impl::SharedAllocationRecord"* @_ZN6Kokkos4Impl22SharedAllocationRecordIvvE9decrementEPS2_(%"class.Kokkos::Impl::SharedAllocationRecord"*) #1

; Function Attrs: noinline optnone uwtable
define linkonce_odr void @_ZNK6Kokkos4Impl23SharedAllocationTracker9get_labelINS_9HostSpaceEEENSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEv(%"class.std::__cxx11::basic_string"* noalias sret %agg.result, %"union.Kokkos::Impl::SharedAllocationTracker"* %this) #2 comdat align 2 {
entry:
  %this.addr = alloca %"union.Kokkos::Impl::SharedAllocationTracker"*, align 8
  store %"union.Kokkos::Impl::SharedAllocationTracker"* %this, %"union.Kokkos::Impl::SharedAllocationTracker"** %this.addr, align 8
  %this1 = load %"union.Kokkos::Impl::SharedAllocationTracker"*, %"union.Kokkos::Impl::SharedAllocationTracker"** %this.addr, align 8
  %m_record_bits = bitcast %"union.Kokkos::Impl::SharedAllocationTracker"* %this1 to i64*
  %0 = load i64, i64* %m_record_bits, align 8
  %cmp = icmp eq i64 %0, 1
  br i1 %cmp, label %cond.true, label %cond.false

cond.true:                                        ; preds = %entry
  call void @_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1Ev(%"class.std::__cxx11::basic_string"* %agg.result) #7
  br label %cond.end

cond.false:                                       ; preds = %entry
  %m_record_bits2 = bitcast %"union.Kokkos::Impl::SharedAllocationTracker"* %this1 to i64*
  %1 = load i64, i64* %m_record_bits2, align 8
  %and = and i64 %1, -2
  %2 = inttoptr i64 %and to %"class.Kokkos::Impl::SharedAllocationRecord.7"*
  %3 = bitcast %"class.Kokkos::Impl::SharedAllocationRecord.7"* %2 to void (%"class.std::__cxx11::basic_string"*, %"class.Kokkos::Impl::SharedAllocationRecord.7"*)***
  %vtable = load void (%"class.std::__cxx11::basic_string"*, %"class.Kokkos::Impl::SharedAllocationRecord.7"*)**, void (%"class.std::__cxx11::basic_string"*, %"class.Kokkos::Impl::SharedAllocationRecord.7"*)*** %3, align 8
  %vfn = getelementptr inbounds void (%"class.std::__cxx11::basic_string"*, %"class.Kokkos::Impl::SharedAllocationRecord.7"*)*, void (%"class.std::__cxx11::basic_string"*, %"class.Kokkos::Impl::SharedAllocationRecord.7"*)** %vtable, i64 0
  %4 = load void (%"class.std::__cxx11::basic_string"*, %"class.Kokkos::Impl::SharedAllocationRecord.7"*)*, void (%"class.std::__cxx11::basic_string"*, %"class.Kokkos::Impl::SharedAllocationRecord.7"*)** %vfn, align 8
  call void %4(%"class.std::__cxx11::basic_string"* sret %agg.result, %"class.Kokkos::Impl::SharedAllocationRecord.7"* %2)
  br label %cond.end

cond.end:                                         ; preds = %cond.false, %cond.true
  ret void
}

; Function Attrs: nounwind
declare void @_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1Ev(%"class.std::__cxx11::basic_string"*) unnamed_addr #6

; Function Attrs: noinline uwtable
define internal void @_GLOBAL__sub_I_forall_kokkos_1.cpp() #5 section ".text.startup" {
entry:
  call void @__cxx_global_var_init()
  ret void
}

declare %struct.__cilkrts_worker* @__cilkrts_get_tls_worker()

declare %struct.__cilkrts_worker* @__cilkrts_bind_thread_1()

declare void @__cilkrts_leave_frame(%struct.__cilkrts_stack_frame*)

; Function Attrs: noinline norecurse optnone uwtable
define internal fastcc void @main_kokkos.forall.body.entry.cilk(i32* align 4 %i4.cilk, %"class.Kokkos::View"* align 8 %a.cilk) #0 personality i8* bitcast (i32 (...)* @__gxx_personality_v0 to i8*) {
kokkos.forall.detach.cilk:
  %__cilkrts_sf = alloca %struct.__cilkrts_stack_frame, align 8
  %0 = call %struct.__cilkrts_worker* @__cilkrts_get_tls_worker() #7
  %1 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 0
  store volatile i32 16777216, i32* %1
  %2 = getelementptr inbounds %struct.__cilkrts_worker, %struct.__cilkrts_worker* %0, i32 0, i32 9
  %3 = load volatile %struct.__cilkrts_stack_frame*, %struct.__cilkrts_stack_frame** %2
  %4 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 2
  store volatile %struct.__cilkrts_stack_frame* %3, %struct.__cilkrts_stack_frame** %4
  %5 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 3
  store volatile %struct.__cilkrts_worker* %0, %struct.__cilkrts_worker** %5
  %6 = getelementptr inbounds %struct.__cilkrts_worker, %struct.__cilkrts_worker* %0, i32 0, i32 9
  store volatile %struct.__cilkrts_stack_frame* %__cilkrts_sf, %struct.__cilkrts_stack_frame** %6
  %7 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 3
  %8 = load volatile %struct.__cilkrts_worker*, %struct.__cilkrts_worker** %7
  %9 = getelementptr inbounds %struct.__cilkrts_worker, %struct.__cilkrts_worker* %8, i32 0, i32 0
  %10 = load volatile %struct.__cilkrts_stack_frame**, %struct.__cilkrts_stack_frame*** %9
  %11 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 9
  %12 = load { %struct.__cilkrts_pedigree }, { %struct.__cilkrts_pedigree }* %11
  %13 = getelementptr inbounds %struct.__cilkrts_worker, %struct.__cilkrts_worker* %8, i32 0, i32 12
  %14 = load %struct.__cilkrts_pedigree, %struct.__cilkrts_pedigree* %13
  %15 = insertvalue { %struct.__cilkrts_pedigree } %12, %struct.__cilkrts_pedigree %14, 0
  %16 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 2
  %17 = load %struct.__cilkrts_stack_frame*, %struct.__cilkrts_stack_frame** %16
  %18 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %17, i32 0, i32 9
  %19 = load { %struct.__cilkrts_pedigree }, { %struct.__cilkrts_pedigree }* %18
  %20 = getelementptr inbounds %struct.__cilkrts_worker, %struct.__cilkrts_worker* %8, i32 0, i32 12
  %21 = load %struct.__cilkrts_pedigree, %struct.__cilkrts_pedigree* %20
  %22 = insertvalue { %struct.__cilkrts_pedigree } %19, %struct.__cilkrts_pedigree %21, 0
  %23 = getelementptr inbounds %struct.__cilkrts_worker, %struct.__cilkrts_worker* %8, i32 0, i32 12
  %24 = getelementptr inbounds %struct.__cilkrts_pedigree, %struct.__cilkrts_pedigree* %23, i32 0, i32 0
  store i64 0, i64* %24
  %25 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 9
  %26 = getelementptr inbounds { %struct.__cilkrts_pedigree }, { %struct.__cilkrts_pedigree }* %25, i32 0, i32 0
  %27 = getelementptr inbounds %struct.__cilkrts_worker, %struct.__cilkrts_worker* %8, i32 0, i32 12
  %28 = getelementptr inbounds %struct.__cilkrts_pedigree, %struct.__cilkrts_pedigree* %27, i32 0, i32 1
  store %struct.__cilkrts_pedigree* %26, %struct.__cilkrts_pedigree** %28
  %29 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 2
  %30 = load volatile %struct.__cilkrts_stack_frame*, %struct.__cilkrts_stack_frame** %29
  store volatile %struct.__cilkrts_stack_frame* %30, %struct.__cilkrts_stack_frame** %10
  %31 = getelementptr %struct.__cilkrts_stack_frame*, %struct.__cilkrts_stack_frame** %10, i32 1
  %32 = getelementptr inbounds %struct.__cilkrts_worker, %struct.__cilkrts_worker* %8, i32 0, i32 0
  store volatile %struct.__cilkrts_stack_frame** %31, %struct.__cilkrts_stack_frame*** %32
  %33 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 0
  %34 = load volatile i32, i32* %33
  %35 = or i32 %34, 4
  %36 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 0
  store volatile i32 %35, i32* %36
  %this.addr.i104.cilk = alloca %"class.Kokkos::View"*, align 8
  %i0.addr.i105.cilk = alloca i32*, align 8
  %i1.addr.i106.cilk = alloca i32*, align 8
  %this.addr.i65.cilk = alloca %"class.Kokkos::View"*, align 8
  %i0.addr.i66.cilk = alloca i32*, align 8
  %i1.addr.i67.cilk = alloca i32*, align 8
  %ref.tmp5.cilk = alloca i32, align 4
  %exn.slot7.cilk = alloca i8*
  %ehselector.slot8.cilk = alloca i32
  %ref.tmp13.cilk = alloca i32, align 4
  br label %kokkos.forall.body.entry.cilk

kokkos.forall.inc.cilk:                           ; preds = %kokkos.forall.preattach.cilk
  %37 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 2
  %38 = load volatile %struct.__cilkrts_stack_frame*, %struct.__cilkrts_stack_frame** %37
  %39 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 3
  %40 = load volatile %struct.__cilkrts_worker*, %struct.__cilkrts_worker** %39
  %41 = getelementptr inbounds %struct.__cilkrts_worker, %struct.__cilkrts_worker* %40, i32 0, i32 9
  store volatile %struct.__cilkrts_stack_frame* %38, %struct.__cilkrts_stack_frame** %41
  %42 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 2
  store volatile %struct.__cilkrts_stack_frame* null, %struct.__cilkrts_stack_frame** %42
  %43 = getelementptr inbounds %struct.__cilkrts_stack_frame, %struct.__cilkrts_stack_frame* %__cilkrts_sf, i32 0, i32 0
  %44 = load volatile i32, i32* %43
  %45 = icmp ne i32 %44, 16777216
  br i1 %45, label %body.i, label %__cilk_parent_epilogue.exit

body.i:                                           ; preds = %kokkos.forall.inc.cilk
  call void @__cilkrts_leave_frame(%struct.__cilkrts_stack_frame* %__cilkrts_sf) #7
  br label %__cilk_parent_epilogue.exit

__cilk_parent_epilogue.exit:                      ; preds = %kokkos.forall.inc.cilk, %body.i
  ret void

kokkos.forall.body.entry.cilk:                    ; preds = %kokkos.forall.detach.cilk
  %46 = bitcast %"class.Kokkos::View"** %this.addr.i104.cilk to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %46)
  %47 = bitcast i32** %i0.addr.i105.cilk to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %47)
  %48 = bitcast i32** %i1.addr.i106.cilk to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %48)
  %49 = bitcast %"class.Kokkos::View"** %this.addr.i65.cilk to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %49)
  %50 = bitcast i32** %i0.addr.i66.cilk to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %50)
  %51 = bitcast i32** %i1.addr.i67.cilk to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %51)
  %52 = bitcast i32* %ref.tmp5.cilk to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %52)
  %53 = bitcast i8** %exn.slot7.cilk to i8*
  call void @llvm.lifetime.start.p0i8(i64 8, i8* %53)
  %54 = bitcast i32* %ehselector.slot8.cilk to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %54)
  %55 = bitcast i32* %ref.tmp13.cilk to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* %55)
  br label %kokkos.forall.pfor.body.cilk

kokkos.forall.pfor.body.cilk:                     ; preds = %kokkos.forall.body.entry.cilk
  %56 = load i32, i32* %i4.cilk, align 4
  %conv.cilk = sitofp i32 %56 to double
  %mul.cilk = fmul double %conv.cilk, 1.000000e+02
  store i32 0, i32* %ref.tmp5.cilk, align 4
  store %"class.Kokkos::View"* %a.cilk, %"class.Kokkos::View"** %this.addr.i65.cilk, align 8
  store i32* %i4.cilk, i32** %i0.addr.i66.cilk, align 8
  store i32* %ref.tmp5.cilk, i32** %i1.addr.i67.cilk, align 8
  %this1.i68.cilk = load %"class.Kokkos::View"*, %"class.Kokkos::View"** %this.addr.i65.cilk, align 8
  %m_map.i69.cilk = getelementptr inbounds %"class.Kokkos::View", %"class.Kokkos::View"* %this1.i68.cilk, i32 0, i32 1
  %m_handle.i70.cilk = getelementptr inbounds %"class.Kokkos::Impl::ViewMapping", %"class.Kokkos::Impl::ViewMapping"* %m_map.i69.cilk, i32 0, i32 0
  %57 = load double*, double** %m_handle.i70.cilk, align 8
  %58 = load i32*, i32** %i1.addr.i67.cilk, align 8
  %59 = load i32, i32* %58, align 4
  %conv.i71.cilk = sext i32 %59 to i64
  %m_map2.i72.cilk = getelementptr inbounds %"class.Kokkos::View", %"class.Kokkos::View"* %this1.i68.cilk, i32 0, i32 1
  %m_offset.i73.cilk = getelementptr inbounds %"class.Kokkos::Impl::ViewMapping", %"class.Kokkos::Impl::ViewMapping"* %m_map2.i72.cilk, i32 0, i32 1
  %m_stride.i74.cilk = getelementptr inbounds %"struct.Kokkos::Impl::ViewOffset", %"struct.Kokkos::Impl::ViewOffset"* %m_offset.i73.cilk, i32 0, i32 1
  %60 = load i64, i64* %m_stride.i74.cilk, align 8
  %61 = load i32*, i32** %i0.addr.i66.cilk, align 8
  %62 = load i32, i32* %61, align 4
  %conv3.i.cilk = sext i32 %62 to i64
  %mul.i75.cilk = mul i64 %60, %conv3.i.cilk
  %add.i76.cilk = add i64 %conv.i71.cilk, %mul.i75.cilk
  %arrayidx.i77.cilk = getelementptr inbounds double, double* %57, i64 %add.i76.cilk
  br label %invoke.cont9.cilk

invoke.cont9.cilk:                                ; preds = %kokkos.forall.pfor.body.cilk
  %63 = load double, double* %arrayidx.i77.cilk, align 8
  %add.cilk = fadd double %63, %mul.cilk
  store double %add.cilk, double* %arrayidx.i77.cilk, align 8
  %64 = load i32, i32* %i4.cilk, align 4
  %conv11.cilk = sitofp i32 %64 to double
  %mul12.cilk = fmul double %conv11.cilk, 1.000000e+04
  store i32 1, i32* %ref.tmp13.cilk, align 4
  store %"class.Kokkos::View"* %a.cilk, %"class.Kokkos::View"** %this.addr.i104.cilk, align 8
  store i32* %i4.cilk, i32** %i0.addr.i105.cilk, align 8
  store i32* %ref.tmp13.cilk, i32** %i1.addr.i106.cilk, align 8
  %this1.i107.cilk = load %"class.Kokkos::View"*, %"class.Kokkos::View"** %this.addr.i104.cilk, align 8
  %m_map.i108.cilk = getelementptr inbounds %"class.Kokkos::View", %"class.Kokkos::View"* %this1.i107.cilk, i32 0, i32 1
  %m_handle.i109.cilk = getelementptr inbounds %"class.Kokkos::Impl::ViewMapping", %"class.Kokkos::Impl::ViewMapping"* %m_map.i108.cilk, i32 0, i32 0
  %65 = load double*, double** %m_handle.i109.cilk, align 8
  %66 = load i32*, i32** %i1.addr.i106.cilk, align 8
  %67 = load i32, i32* %66, align 4
  %conv.i110.cilk = sext i32 %67 to i64
  %m_map2.i111.cilk = getelementptr inbounds %"class.Kokkos::View", %"class.Kokkos::View"* %this1.i107.cilk, i32 0, i32 1
  %m_offset.i112.cilk = getelementptr inbounds %"class.Kokkos::Impl::ViewMapping", %"class.Kokkos::Impl::ViewMapping"* %m_map2.i111.cilk, i32 0, i32 1
  %m_stride.i113.cilk = getelementptr inbounds %"struct.Kokkos::Impl::ViewOffset", %"struct.Kokkos::Impl::ViewOffset"* %m_offset.i112.cilk, i32 0, i32 1
  %68 = load i64, i64* %m_stride.i113.cilk, align 8
  %69 = load i32*, i32** %i0.addr.i105.cilk, align 8
  %70 = load i32, i32* %69, align 4
  %conv3.i114.cilk = sext i32 %70 to i64
  %mul.i115.cilk = mul i64 %68, %conv3.i114.cilk
  %add.i116.cilk = add i64 %conv.i110.cilk, %mul.i115.cilk
  %arrayidx.i117.cilk = getelementptr inbounds double, double* %65, i64 %add.i116.cilk
  br label %invoke.cont14.cilk

invoke.cont14.cilk:                               ; preds = %invoke.cont9.cilk
  %71 = load double, double* %arrayidx.i117.cilk, align 8
  %add16.cilk = fadd double %71, %mul12.cilk
  store double %add16.cilk, double* %arrayidx.i117.cilk, align 8
  br label %kokkos.forall.preattach.cilk

kokkos.forall.preattach.cilk:                     ; preds = %invoke.cont14.cilk
  br label %kokkos.forall.inc.cilk
}

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.start.p0i8(i64, i8* nocapture) #3

; Function Attrs: nounwind readnone
declare i8* @llvm.frameaddress(i32) #11

; Function Attrs: nounwind
declare i8* @llvm.stacksave() #7

; Function Attrs: nounwind
declare i32 @llvm.eh.sjlj.setjmp(i8*) #7

declare void @__cilkrts_sync(%struct.__cilkrts_stack_frame*)

declare void @__cilkrts_rethrow(%struct.__cilkrts_stack_frame*)

attributes #0 = { noinline norecurse optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { noinline optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { argmemonly nounwind }
attributes #4 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { noinline uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #6 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #7 = { nounwind }
attributes #8 = { nobuiltin "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #9 = { nobuiltin nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #10 = { noinline noreturn nounwind }
attributes #11 = { nounwind readnone }
attributes #12 = { returns_twice }
attributes #13 = { noreturn }
attributes #14 = { noreturn nounwind }
attributes #15 = { builtin }
attributes #16 = { builtin nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 5.0.0 "}
!2 = distinct !{!2, !3}
!3 = !{!"tapir.loop.spawn.strategy", i32 1}
