! This reads single data files which have a header of 7680 bytes
! Kay Diederichs 4/2017
! Kay Diederichs 7/2021 add code for the case that fn_template has no '?', and simplify&comment gfortran command.
!
! compile with
! ifort -fpic -shared -static-intel -qopenmp -qopenmp-link=static -traceback -sox test_generic_client.f90 -o libtest_generic_client.so
! (this includes all required compiler libraries into the libtest_generic_client.so library)
! or
! gfortran -fpic test_generic_client.f90 -shared -o libtest_generic_client.so
! (this does not include the compiler's libgfortran.so and libquadmath.so into the library; don't know how to achieve this so
! gfortran is only useful if it is anyway installed on the machine)
! The resulting file can be used with a LIB=./libtest_generic_client.so line in XDS.INP, and enables
! reading of data files with a 7680 bytes header plus 1024*1024 pixels of integer data, without any record structure.

MODULE plugin_test_mod
       CHARACTER :: fn_template*132='',cformat*6='(i4.4)'
       INTEGER   :: lenfn,firstqm,lastqm
END MODULE

SUBROUTINE plugin_open(filename, info_array, error_flag) bind(C)
       USE ISO_C_BINDING
       USE plugin_test_mod
       integer(c_int)                  :: error_flag
       character(kind=c_char)          :: filename(*)
       integer(c_int), dimension(1024) :: info_array
       INTEGER i
       
       DO i=1,LEN(fn_template)
         IF (filename(i)==C_NULL_CHAR) EXIT
         fn_template(i:i)=filename(i)
       END DO
       WRITE(*,*)'libtest_generic_client v1.0; Kay Diederichs 20.4.17'
       WRITE(*,*)'plugin_open: fn_template=',TRIM(fn_template)
       lenfn=LEN_TRIM(fn_template)
       info_array=0
       error_flag=0
       firstqm=INDEX(fn_template,'?')
       lastqm =INDEX(fn_template,'?',BACK=.TRUE.)
       IF (firstqm==0) THEN
         firstqm=lenfn-7
         lastqm =lenfn-4
       END IF
       WRITE(cformat(3:5),'(i1,a1,i1)')lastqm-firstqm+1,'.',lastqm-firstqm+1
END SUBROUTINE plugin_open
!
subroutine plugin_get_header(nx, ny, nbyte, qx, qy, number_of_frames, info_array, error_flag) bind(C)
       USE ISO_C_BINDING
       integer(c_int)                  :: nx, ny, nbyte, number_of_frames       
       real(c_float)                   :: qx, qy
       integer(c_int)                  :: error_flag
       integer(c_int), dimension(1024) :: info_array
       
!       WRITE(*,*)'plugin_get_header was called'
       nx=1024
       ny=1024
       nbyte=4
       qx=0.172
       qy=0.172
       number_of_frames=9999
       info_array=0
       info_array(1)=0
       error_flag=0
END SUBROUTINE plugin_get_header
!
SUBROUTINE plugin_get_data(frame_number, nx, ny, data_array, info_array, error_flag)  BIND(C,NAME="plugin_get_data")
    USE ISO_C_BINDING
    USE plugin_test_mod
    integer(c_int)                    :: nx, ny, frame_number
    integer(c_int)                    :: error_flag
    integer(c_int), dimension(1024)   :: info_array
    integer(c_int), dimension (nx*ny) :: data_array
! local variables
    INTEGER k,i,dummy
    CHARACTER :: fn*132
    fn=fn_template
    IF (frame_number>0) WRITE(fn(firstqm:lastqm),cformat) frame_number
! -qopenmp compile option needs to be used otherwise race in writing fn
    OPEN(newunit=k,file=fn,action='READ',ACCESS='STREAM',form='unformatted')
    WRITE(*,*)'plugin_get_data was called; frame_number,k=',frame_number,k
    READ(k)(dummy,i=1,1920) ! read 15*512=7680 header bytes 
    READ(k) data_array
    CLOSE(k)
    error_flag=0
END SUBROUTINE plugin_get_data
!
SUBROUTINE plugin_close(error_flag) BIND(C,NAME="plugin_close")
    USE ISO_C_BINDING
    integer(c_int)                     :: error_flag
!    WRITE(*,*)'plugin_close was called'
    error_flag=0
END SUBROUTINE plugin_close

