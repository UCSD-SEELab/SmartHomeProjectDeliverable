
G
input/tv_plugPlaceholder*
dtype0*
shape:���������
9
input/keepprobPlaceholder*
dtype0*
shape:
�
connect_sensor_0/tv_plug/w_0Const*�
value�B�@"�S�{>��v��Hc>:^>5DD�pA>97>O�]�=�X4>�C��8a���#G>�=�ꜽ�j�v�.=��?��,��-?n>�v�=-z>���=�Z�>�y0>�<="���~+�����z>��<[�<��=�m>�T>�q>�����QB>��ٽ�=M���>ʼh�����􈱻�����=S;���O�<�Jؽ�ޑ�e��r���*꒼s�(>���<=HU���.>]�=7�>����<����/�@=��:=]_��H�A�[x'�&F��n'�<��<<I����=`}�=��C;��X��X�����<�;Ql�=g(�ٴ��O��:�I<�iƀ<Б�=�򚽞\�=�%�=���=ǝ��]C�=��y=�$���P	�ɚ��ԟp�^�7�0�d=���!g�;u�=��:��>x=��8���|=wS���e��<Q�<Uo�=�M�<��=�2��ľ<��t�����<;X�����<��"7�O�=ϣI�K�=4O�5�&����	=u!�=*
dtype0
�
!connect_sensor_0/tv_plug/w_0/readIdentityconnect_sensor_0/tv_plug/w_0*
T0*/
_class%
#!loc:@connect_sensor_0/tv_plug/w_0
�
connect_sensor_0/tv_plug/bias_0Const*�
value�B�@"�5�=�Q�<7�=�n�=��
��=D1m=¥�=�/I=�s�=�	G=�Z=*]�=�	%=�b|�����D�ǽ�Q�=E��=��=�l�="�=�7���>~�=jU���p��m�*=�X��X9�=�/��臻���1Ĺ=�k1��F�=���=���=��u=:ٜ=
D�=��-���;@W����=p�"={��výL�v=!$!�S�J��Rn=���<g��=4�<�E�<��5��W�=�Yd=Oc>��׼\)��UR���*
dtype0
�
$connect_sensor_0/tv_plug/bias_0/readIdentityconnect_sensor_0/tv_plug/bias_0*
T0*2
_class(
&$loc:@connect_sensor_0/tv_plug/bias_0
�
!connect_sensor_0_1/tv_plug/MatMulMatMulinput/tv_plug!connect_sensor_0/tv_plug/w_0/read*
T0*
transpose_a( *
transpose_b( 
w
connect_sensor_0_1/tv_plug/addAdd!connect_sensor_0_1/tv_plug/MatMul$connect_sensor_0/tv_plug/bias_0/read*
T0
P
connect_sensor_0_1/tv_plug/ReluReluconnect_sensor_0_1/tv_plug/add*
T0
k
(connect_sensor_0_1/tv_plug/dropout/ShapeShapeconnect_sensor_0_1/tv_plug/Relu*
T0*
out_type0
b
5connect_sensor_0_1/tv_plug/dropout/random_uniform/minConst*
valueB
 *    *
dtype0
b
5connect_sensor_0_1/tv_plug/dropout/random_uniform/maxConst*
valueB
 *  �?*
dtype0
�
?connect_sensor_0_1/tv_plug/dropout/random_uniform/RandomUniformRandomUniform(connect_sensor_0_1/tv_plug/dropout/Shape*
T0*
dtype0*
seed2 *

seed 
�
5connect_sensor_0_1/tv_plug/dropout/random_uniform/subSub5connect_sensor_0_1/tv_plug/dropout/random_uniform/max5connect_sensor_0_1/tv_plug/dropout/random_uniform/min*
T0
�
5connect_sensor_0_1/tv_plug/dropout/random_uniform/mulMul?connect_sensor_0_1/tv_plug/dropout/random_uniform/RandomUniform5connect_sensor_0_1/tv_plug/dropout/random_uniform/sub*
T0
�
1connect_sensor_0_1/tv_plug/dropout/random_uniformAdd5connect_sensor_0_1/tv_plug/dropout/random_uniform/mul5connect_sensor_0_1/tv_plug/dropout/random_uniform/min*
T0
y
&connect_sensor_0_1/tv_plug/dropout/addAddinput/keepprob1connect_sensor_0_1/tv_plug/dropout/random_uniform*
T0
b
(connect_sensor_0_1/tv_plug/dropout/FloorFloor&connect_sensor_0_1/tv_plug/dropout/add*
T0
k
&connect_sensor_0_1/tv_plug/dropout/divRealDivconnect_sensor_0_1/tv_plug/Reluinput/keepprob*
T0
�
&connect_sensor_0_1/tv_plug/dropout/mulMul&connect_sensor_0_1/tv_plug/dropout/div(connect_sensor_0_1/tv_plug/dropout/Floor*
T0
�
!connect_sensor_0/tv_plug/w_outputConst*�
value�B�@"������+>qh<�U����.���>M�&����=y���4�<�w�i6<;����. >�l	=p
���k��7=��_���=�B�;a��9W�4=��Z=�XQ�,:>?�i���:=6���A�j-"��@��i�<r�a�m�=��a=�e�=�Ɩ=\;J�i>�� �M��=���o�>�������B{�t!^>*[��\�>k��=\M";8r���<>0�<�Su��=�z�A���y>�wt=K=�^��=�03�=?:��Տ��m��7��C�%N�T�(>tt�<���r�����=�_�=��̻N�=4O��[q��t��=��涶g��ᵼq< ���r��<^�^=p��9򡅽ԈQ=6zK��%�=9�=����� �=(;Ży����<�ļ���V<,=�𞼾��;�>[��{�Ώ�=���;4N���ㄼV>�=M8���T�;����/)>�FB<��=�0��V�=k~�:e�=��R<�T��~�C7�:b��=��<*
dtype0
�
&connect_sensor_0/tv_plug/w_output/readIdentity!connect_sensor_0/tv_plug/w_output*
T0*4
_class*
(&loc:@connect_sensor_0/tv_plug/w_output
Y
$connect_sensor_0/tv_plug/bias_outputConst*
dtype0*
valueB"����=
�
)connect_sensor_0/tv_plug/bias_output/readIdentity$connect_sensor_0/tv_plug/bias_output*
T0*7
_class-
+)loc:@connect_sensor_0/tv_plug/bias_output
�
#connect_sensor_0_1/tv_plug_1/MatMulMatMul&connect_sensor_0_1/tv_plug/dropout/mul&connect_sensor_0/tv_plug/w_output/read*
T0*
transpose_a( *
transpose_b( 
�
 connect_sensor_0_1/tv_plug_1/addAdd#connect_sensor_0_1/tv_plug_1/MatMul)connect_sensor_0/tv_plug/bias_output/read*
T0
b
+connect_sensor_0_1/tv_plug_1/tv_plug_outputIdentity connect_sensor_0_1/tv_plug_1/add*
T0
R
input/livingroomIdentity+connect_sensor_0_1/tv_plug_1/tv_plug_output*
T0
�
livingroom/w_0Const*�
value�B�@"�D9���0p�Ŭ�<�;��B�׽��E=z	;v��J0=S�0=D�[��G��U�<)�<(9���м=�Dp�ci�����sR��˽��=l6�<�����=4�ٽ%_=�%�=H*���+��;rY(����<y

�#jx�h���j<�B�na;��ٽ��<E�v<�Պ����:��<��<�;��e����oy=b���J�;��]�D���S���=t9J�~=���e0�L���YR<�����A>���<�g��=� �=���R%�M��=kͼ�i; UO>'���J!��f���A��P�=��=���=���=��#��=�O=6� =� �� �=�:S��=]������<e�9=l}a�Aۣ�~¸=cՃ�N��=�j����ݻ~'���4>�#��?�=��Z������A>�� �{ �4�ǻk%>�p���歼?D=a)�=��=���=V�P��2�л�h�W��<��>֧�=���=(Pf��	�=*
dtype0
[
livingroom/w_0/readIdentitylivingroom/w_0*
T0*!
_class
loc:@livingroom/w_0
�
livingroom/bias_0Const*�
value�B�@"�/X�=ף�=|����=��=U�Q=OK=݈<��ȼo�=�#z=B�_=3��y��?�h� ,%=�O�6,�=�Y=�d˼6-E��;�� =�Jü�&�=}{=��;��I<P�=��[���E��=���=U/��Q4"���>���6=�2=�=Y�h=�n&<9w�=�d�=�b.=lw����=�_o=�sv=����Y;�<W=���<0=Dz=�ҼSL���]=ha��_ͼy�;ꙁ=�љ=� ��ڶ�=*
dtype0
d
livingroom/bias_0/readIdentitylivingroom/bias_0*
T0*$
_class
loc:@livingroom/bias_0
q
livingroom/MatMulMatMulinput/livingroomlivingroom/w_0/read*
transpose_b( *
T0*
transpose_a( 
I
livingroom/addAddlivingroom/MatMullivingroom/bias_0/read*
T0
0
livingroom/ReluRelulivingroom/add*
T0
K
livingroom/dropout/ShapeShapelivingroom/Relu*
T0*
out_type0
R
%livingroom/dropout/random_uniform/minConst*
valueB
 *    *
dtype0
R
%livingroom/dropout/random_uniform/maxConst*
valueB
 *  �?*
dtype0
�
/livingroom/dropout/random_uniform/RandomUniformRandomUniformlivingroom/dropout/Shape*
T0*
dtype0*
seed2 *

seed 
�
%livingroom/dropout/random_uniform/subSub%livingroom/dropout/random_uniform/max%livingroom/dropout/random_uniform/min*
T0
�
%livingroom/dropout/random_uniform/mulMul/livingroom/dropout/random_uniform/RandomUniform%livingroom/dropout/random_uniform/sub*
T0

!livingroom/dropout/random_uniformAdd%livingroom/dropout/random_uniform/mul%livingroom/dropout/random_uniform/min*
T0
Y
livingroom/dropout/addAddinput/keepprob!livingroom/dropout/random_uniform*
T0
B
livingroom/dropout/FloorFloorlivingroom/dropout/add*
T0
K
livingroom/dropout/divRealDivlivingroom/Reluinput/keepprob*
T0
X
livingroom/dropout/mulMullivingroom/dropout/divlivingroom/dropout/Floor*
T0
�
livingroom/w_outputConst*�
value�B�@"����t9<���=S���Xf�<���.�=��y;m�<��!��%�7�����4����V�=d;n��Q"��̽��=S|���Ƿ<�B�=�Щ�V�;�'�<��z;�ø�H7�;�۽�ޥ�H =�i����n��e=�L�*���>q=��o�|N�=��<�6��Ȫo�[P>��<��_M��r7�9z�=�������^�%��c��{&0��Q5�<m:��/)�d�z�y<wp��*�<�����W�t=�������ߤ=���:��k=|�aW��0��=Y�?<+
��E�$���%���;=[����o��E�=�)���E}=p�<���������=��=NJǽ�C�<�.�"/�=�T����+�ە~�f홒s��*N���	����=hЦ�J�8=�Ƨ:��$����<����6%i���=�?���;���:�dȹ;4�=��c=C�=��=Y�X=�/���v'=��=hdg��fl<2��<63=k�=qw�;z��<h��;�6������=j�߽jֹ/�|���'M<'i���n�+;��ى=�_��s�M�}�e�hk=��M�<�_����g��=:=6��*<�<P���G;�����G4>���@jU<�w��L=�_=;w�����<[t�=���=hzo;i1<��;�1=�A=I�|=j�A�����K�}�->C���h픻f;V���&��`�<�|@;{��c�;��{<�(C���U;���;o5��P^D�εS>��	�����r �=fw.����J=w
;��<�κԅ�=�1H:�-;%3�<�̽jǽ�_�=�H�=s������)�������4��6�j�P��=���W�G0= ��';�ų���N;l=�m��(7��P�=J7�=�48�悑=P����f�=��2:�5O�E�ùq�b:�U�e�B=r��i�=B5>?��k���i� M>JQ������6��7�=
n�W�\���9;+̓����=s�+=�'!�i� >�`I�*
dtype0
j
livingroom/w_output/readIdentitylivingroom/w_output*
T0*&
_class
loc:@livingroom/w_output
S
livingroom/bias_outputConst*%
valueB"l��=�P��JU�=[�t�*
dtype0
s
livingroom/bias_output/readIdentitylivingroom/bias_output*
T0*)
_class
loc:@livingroom/bias_output
~
livingroom_1/MatMulMatMullivingroom/dropout/mullivingroom/w_output/read*
T0*
transpose_a( *
transpose_b( 
R
livingroom_1/addAddlivingroom_1/MatMullivingroom/bias_output/read*
T0
E
livingroom_1/livingroom_outputIdentitylivingroom_1/add*
T0 