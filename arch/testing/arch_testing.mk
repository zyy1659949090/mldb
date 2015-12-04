# This file is part of MLDB. Copyright 2015 Datacratic. All rights reserved.

$(eval $(call test,tick_counter_test,arch,boost))
$(eval $(call test,bitops_test,arch,boost))
$(eval $(call test,cpuid_test,arch,boost))
$(eval $(call test,futex_test,arch,boost))
$(eval $(call test,fslock_test,arch,boost))
$(eval $(call test,simd_test,arch,boost))
$(eval $(call test,cmp_xchg_test,arch boost_thread boost_system,boost))
$(eval $(call test,simd_vector_test,arch,boost))
$(eval $(call test,simd_vector_benchmark,arch,boost manual))
$(eval $(call test,backtrace_test,arch,boost))
$(eval $(call test,bit_range_ops_test,arch,boost))
$(eval $(call test,atomic_ops_test,arch boost_thread,boost))
$(eval $(call test,sse2_math_test,arch,boost))
$(eval $(call test,vm_test,arch,boost manual)) # latest linux path make this test fail https://lwn.net/Articles/642074/
$(eval $(call test,info_test,arch,boost))
$(eval $(call test,rtti_utils_test,arch,boost))
$(eval $(call test,thread_specific_test,arch boost_thread,boost))
$(eval $(call test,gc_test,gc,boost))
$(eval $(call test,rcu_protected_test,gc,boost timed))
