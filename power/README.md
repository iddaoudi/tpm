* CHOLESKY | QR (4 different tasks)
- 1  potrf						| ormqr
- 2  gemm		   			| tsqrt
- 3  trsm		   			| tsmqr
- 4  syrk		   			| geqrt
- 5  potrf gemm		   	| ormqr tsqrt
- 6  potrf trsm		   	| ormqr tsmqr
- 7  potrf syrk		   	| ormqr geqrt
- 8  gemm trsm		   		| tsqrt tsmqr
- 9  gemm syrk		   		| tsqrt geqrt
- 10 trsm syrk		   		| tsmqr geqrt
- 11 potrf gemm trsm 	   | ormqr tsqrt tsmqr
- 12 potrf gemm syrk	   	| ormqr tsqrt geqrt
- 13 potrf trsm syrk	   	| ormqr tsmqr geqrt
- 14 gemm trsm syrk	   	| tsqrt tsmqr geqrt
- 15 potrf trsm syrk gemm  | ormqr tsmqr geqrt tsqrt
- 16 native

* LU (3 different tasks)
- 1 getrf
- 2 gemm
- 3 trsm
- 4 getrf trsm
- 5 getrf gemm
- 6 trsm gemm
- 7 getrf trsm gemm
- 8 native
