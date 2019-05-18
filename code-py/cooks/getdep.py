import re
import subprocess


# result from 'git show master/U33.3.56:Resolve_Deps/ini_files/resolve_deps.ini'
# git ssh://wwwin-git-gpk-2.cisco.com:29418/ee_jenkins master
# git ssh://wwwin-git-gpk-2.cisco.com:29418/ci2-client master
# ivy aem_bin
# ivy ant_1.6_linux
# ivy auto_mock
# ivy bem_bin
# ivy cappres_bin
# ivy captrans_bin
# ivy ctm_bin
# ivy darwin_dlm_bin
# ivy darwin_ofs_bin
# ivy darwin_player_bin
# ivy darwin_prefs_bin
# ivy darwin_spm_bin
# ivy darwin_srm_bin
# ivy dms_build_bin
# ivy doxymock
# ...
# ivy LIBCURL

gitcmds = r'git show master/U33.3.56:Resolve_Deps/ini_files/resolve_deps.ini'
cmds = gitcmds.split()

try:
  p = subprocess.Popen(cmds, stdout=subprocess.PIPE)
except OSError:
  exit

# communicate returns py-tuple
# ('git ssh://wwwin-git-gpk-2.cisco.com:29418/ee_jenkins master\ngit ssh://wwwin-git-gpk-2.cisco.com:29418/ci2-client master\nivy aem_bin\nivy ant_1.6_linux\nivy auto_mock\nivy bem_bin\nivy cappres_bin\nivy captrans_bin\nivy ctm_bin\nivy darwin_dlm_bin\nivy darwin_ofs_bin\nivy darwin_player_bin\nivy darwin_prefs_bin\nivy darwin_spm_bin\nivy darwin_srm_bin\nivy dms_build_bin\nivy doxymock\nivy CINT\nivy CUnit-2.0-2\nivy CppUTest-3.8\nivy GoogleTest-1.8.1\nivy dbengine_bin\nivy diag_bin\nivy gcov\nivy lcov\nivy lcov_experimental\nivy gfx_bin\nivy http_bin\nivy httpserver_bin\nivy JPEG\nivy jdk_1.6_linux\nivy IronPython2.7\nivy labrat\nivy LIBCURL\nivy mcm_bin\nivy mdm_bin\nivy media_streamer_bin\nivy mono\nivy mpm_bin\nivy msm_bin\nivy ncm_bin\nivy nds_gdbsrv\nivy oigc_bin\nivy pcat_bin\nivy ppcm_bin\nivy psched_bin\nivy PS_FONTS\nivy PS_IMAGES\nivy python2.7_linux\nivy rmf_bin\nivy rnc_bin\nivy sdm_bin\nivy sim_bin\nivy ssl_bin\nivy ssm_bin\nivy systemalarm_bin\nivy systemutil_bin\nivy TOOLS_FUSION_PROFILE\nivy USM\nivy VGDRM\nivy VGV_DARWIN\nivy VGV_SKIT\nivy VGV_SKIT_GATEWAY\nivy VGV_SKYEU\nivy VGV_SKYEU_2017\nivy VGV_ETHAN_HIP\nivy VGV_TOTT\nivy VGV_UNIFIED\nivy VGV_VGW\nivy vrm_bin\nivy doxygen_win32\n', None)

result = p.communicate()
# print(result)

# convert one long tuple element, result[0], into py-list

output = str(result[0]).splitlines()
# print(output)

dependencies = []
ivy_show_strings = []
 
for line in output:
  # print(line)
  if line.startswith("ivy"):
    ivy_show_strings.append(('master/U33.3.56' + ':' + "Resolve_Deps/ivy_files/download_%s_ivy.xml") % line[4:])
  else:
    dependencies.append(line)
 
print("dep")
print(dependencies)
print("ivy")
print(ivy_show_strings)

# dep
# ['git ssh://wwwin-git-gpk-2.cisco.com:29418/ee_jenkins master', 'git ssh://wwwin-git-gpk-2.cisco.com:29418/ci2-client master']
# ivy
# ['master/U33.3.56:Resolve_Deps/ivy_files/download_aem_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_ant_1.6_linux_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_auto_mock_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_bem_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_cappres_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_captrans_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_ctm_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_darwin_dlm_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_darwin_ofs_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_darwin_player_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_darwin_prefs_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_darwin_spm_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_darwin_srm_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_dms_build_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_doxymock_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_CINT_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_CUnit-2.0-2_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_CppUTest-3.8_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_GoogleTest-1.8.1_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_dbengine_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_diag_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_gcov_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_lcov_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_lcov_experimental_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_gfx_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_http_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_httpserver_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_JPEG_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_jdk_1.6_linux_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_IronPython2.7_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_labrat_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_LIBCURL_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_mcm_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_mdm_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_media_streamer_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_mono_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_mpm_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_msm_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_ncm_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_nds_gdbsrv_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_oigc_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_pcat_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_ppcm_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_psched_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_PS_FONTS_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_PS_IMAGES_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_python2.7_linux_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_rmf_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_rnc_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_sdm_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_sim_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_ssl_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_ssm_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_systemalarm_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_systemutil_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_TOOLS_FUSION_PROFILE_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_USM_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_VGDRM_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_VGV_DARWIN_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_VGV_SKIT_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_VGV_SKIT_GATEWAY_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_VGV_SKYEU_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_VGV_SKYEU_2017_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_VGV_ETHAN_HIP_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_VGV_TOTT_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_VGV_UNIFIED_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_VGV_VGW_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_vrm_bin_ivy.xml', 'master/U33.3.56:Resolve_Deps/ivy_files/download_doxygen_win32_ivy.xml']

# ['master/U33.3.56:Resolve_Deps/ivy_files/download_aem_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_ant_1.6_linux_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_auto_mock_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_bem_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_cappres_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_captrans_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_ctm_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_darwin_dlm_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_darwin_ofs_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_darwin_player_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_darwin_prefs_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_darwin_spm_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_darwin_srm_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_dms_build_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_doxymock_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_CINT_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_CUnit-2.0-2_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_CppUTest-3.8_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_GoogleTest-1.8.1_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_dbengine_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_diag_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_gcov_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_lcov_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_lcov_experimental_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_gfx_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_http_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_httpserver_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_JPEG_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_jdk_1.6_linux_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_IronPython2.7_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_labrat_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_LIBCURL_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_mcm_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_mdm_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_media_streamer_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_mono_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_mpm_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_msm_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_ncm_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_nds_gdbsrv_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_oigc_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_pcat_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_ppcm_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_psched_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_PS_FONTS_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_PS_IMAGES_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_python2.7_linux_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_rmf_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_rnc_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_sdm_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_sim_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_ssl_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_ssm_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_systemalarm_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_systemutil_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_TOOLS_FUSION_PROFILE_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_USM_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_VGDRM_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_VGV_DARWIN_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_VGV_SKIT_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_VGV_SKIT_GATEWAY_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_VGV_SKYEU_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_VGV_SKYEU_2017_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_VGV_ETHAN_HIP_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_VGV_TOTT_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_VGV_UNIFIED_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_VGV_VGW_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_vrm_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_doxygen_win32_ivy.xml']


# git show master/U33.3.56:Resolve_Deps/ivy_files/download_LIBCURL_ivy.xml

# 'master/U33.3.56:Resolve_Deps/ivy_files/download_http_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_httpserver_bin_ivy.xml',
# 'master/U33.3.56:Resolve_Deps/ivy_files/download_LIBCURL_ivy.xml',

# kyoupark@st-castor-03:~/STB_SW_o$ git show BSKYB_GM_Integration/BSKYB_INTEGRATION_BLD_42.62.00_BSKYB_GM_Integration:Resolve_Deps/ivy_files/download_http_bin_ivy.xml
# <?xml version="1.0" encoding="UTF-8"?>
# <ivy-module version="2.0">
#   <info organisation="middleware" module="bin_http_bin"/>
#        <dependencies>
# <dependency org="middleware" name="http_bin" rev="HTTP_F74.1.REL_HTTP_MAIN_Int">
# <artifact name="http_bin" ext="tar.gz" type="gz"/>
#         </dependency>
#         </dependencies>
# </ivy-module>

# gitcmds = r'git show master/U33.3.56:Resolve_Deps/ivy_files/download_aem_bin_ivy.xml'
# cmds = gitcmds.split()
# 
# p = subprocess.Popen(cmds, stdout=subprocess.PIPE)
# result = p.communicate()
# print("result")
# print(result[0])
# output = str(result[0])
# print("output")
# print(output)
# 
# # dependencies += re.findall(r'rev="([\w.]+)"', output) 
# dependencies = re.findall(r'rev="([\w.]+)"', output) 
# print(dependencies)
