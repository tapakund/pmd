/*
 * Copyright © 2016-2019 VMware, Inc.  All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.  You may obtain a copy
 * of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, without
 * warranties or conditions of any kind, EITHER EXPRESS OR IMPLIED.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include "includes.h"

uint32_t
pkg_open_handle(
    PPMDHANDLE hHandle,
    PTDNF_CMD_ARGS pArgs,
    PPKGHANDLE *phPkgHandle
    )
{
    uint32_t dwError = 0;
    pkg_handle_t hPkgHandle = NULL;
    PTDNF_RPC_CMD_ARGS pRpcArgs = NULL;

    dwError = pkg_get_rpc_cmd_args(pArgs, &pRpcArgs);
    BAIL_ON_PMD_ERROR(dwError);

    if(hHandle->nPrivSep)
    {
        DO_RPC(pkg_privsep_rpc_open_handle(
                   hHandle->hRpc,
                   pRpcArgs,
                   &hPkgHandle), dwError);
    }
    else
    {
        DO_RPC(pkg_rpc_open_handle(
                   hHandle->hRpc,
                   pRpcArgs,
                   &hPkgHandle), dwError);
    }
    BAIL_ON_PMD_ERROR(dwError);

    *phPkgHandle = hPkgHandle;

cleanup:
    free_pkg_rpc_cmd_args(pRpcArgs);
    return dwError;

error:
    if(phPkgHandle)
    {
        *phPkgHandle = NULL;
    }
    goto cleanup;
}

uint32_t
pkg_close_handle(
    PPMDHANDLE hHandle,
    PPKGHANDLE hPkgHandle
    )
{
    uint32_t dwError = 0;

    if(hHandle->nPrivSep)
    {
        DO_RPC(pkg_privsep_rpc_close_handle(
                   hHandle->hRpc,
                   hPkgHandle), dwError);
        BAIL_ON_PMD_ERROR(dwError);
    }
    else
    {
        DO_RPC(pkg_rpc_close_handle(
                   hHandle->hRpc,
                   hPkgHandle), dwError);
        BAIL_ON_PMD_ERROR(dwError);
    }

error:
    return dwError;
}

uint32_t
pkg_clean_w(
    PPMDHANDLE hHandle,
    PPKGHANDLE hPkgHandle,
    TDNF_CLEANTYPE nCleanType,
    PTDNF_RPC_CLEAN_INFO* ppCleanInfo
    )
{
    uint32_t dwError = 0;
    if(hHandle->nPrivSep)
    {
        DO_RPC(pkg_privsep_rpc_clean(hHandle->hRpc,
                                     hPkgHandle,
                                     nCleanType,
                                     ppCleanInfo),
                                     dwError);
    }
    else
    {
        DO_RPC(pkg_rpc_clean(hHandle->hRpc, hPkgHandle, nCleanType, ppCleanInfo),
               dwError);
    }
    BAIL_ON_PMD_ERROR(dwError);
cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
pkg_clean(
    PPMDHANDLE hHandle,
    PPKGHANDLE hPkgHandle,
    TDNF_CLEANTYPE nCleanType,
    PTDNF_CLEAN_INFO* ppCleanInfo
    )
{
    uint32_t dwError = 0;
    PTDNF_RPC_CLEAN_INFO pRpcCleanInfo = NULL;
    PTDNF_CLEAN_INFO pCleanInfo = NULL;
    char **pwszReposUsed = NULL;
    dwError = pkg_clean_w(hHandle, hPkgHandle, nCleanType, &pRpcCleanInfo);
    BAIL_ON_PMD_ERROR(dwError);

    dwError = PMDRpcClientConvertCleanInfo(pRpcCleanInfo, &pCleanInfo);
    BAIL_ON_PMD_ERROR(dwError);

    *ppCleanInfo = pCleanInfo;
cleanup:
    if (pRpcCleanInfo && pRpcCleanInfo->pszReposUsed != NULL)
    {
        int i = 0;
        for (i = 0; i < pRpcCleanInfo->pszReposUsed->dwCount; i++)
        {
            PMD_SAFE_FREE_MEMORY(pRpcCleanInfo->pszReposUsed->ppwszStrings[i]);
        }
        PMD_SAFE_FREE_MEMORY(pRpcCleanInfo->pszReposUsed->ppwszStrings);
        PMD_SAFE_FREE_MEMORY(pRpcCleanInfo->pszReposUsed);
    }
    PMD_SAFE_FREE_MEMORY(pRpcCleanInfo);
    return dwError;
error:
    if(ppCleanInfo)
    {
        *ppCleanInfo = NULL;
    }
    if(pCleanInfo)
    {
        if (pCleanInfo->ppszReposUsed != NULL)
        {
            pwszReposUsed = pCleanInfo->ppszReposUsed;
            while(pwszReposUsed && *pwszReposUsed)
            {
                PMD_SAFE_FREE_MEMORY(*pwszReposUsed);
                ++pwszReposUsed;
            }
        }
        PMD_SAFE_FREE_MEMORY(pCleanInfo->ppszReposUsed);
        PMD_SAFE_FREE_MEMORY(pCleanInfo);
    }
    goto cleanup;

}
uint32_t
pkg_check_local_w(
    PPMDHANDLE hHandle,
    PPKGHANDLE hPkgHandle,
    wstring_t pwszFolder
    )
{

    uint32_t dwError = 0;

    if(hHandle->nPrivSep)
    {
        DO_RPC(pkg_privsep_rpc_check_local(hHandle->hRpc,
                                           hPkgHandle,
                                           pwszFolder),
                                           dwError);
    }
    else
    {
        DO_RPC(pkg_rpc_check_local(hHandle->hRpc, hPkgHandle, pwszFolder),
               dwError);
    }
    BAIL_ON_PMD_ERROR(dwError);
cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
pkg_check_local(
    PPMDHANDLE hHandle,
    PPKGHANDLE hPkgHandle,
    const char *pszFolder
    )
{
    uint32_t dwError = 0;
    wstring_t pwszFolder = NULL;

    dwError = PMDAllocateStringWFromA(
                  pszFolder,
                  &pwszFolder);
    BAIL_ON_PMD_ERROR(dwError);

    dwError = pkg_check_local_w(hHandle, hPkgHandle, pwszFolder);
    BAIL_ON_PMD_ERROR(dwError);

cleanup:
    PMD_SAFE_FREE_MEMORY(pwszFolder);
    return dwError;
error:
    goto cleanup;
}


uint32_t
pkg_search_w(
    PPMDHANDLE hHandle,
    PPKGHANDLE hPkgHandle,
    PTDNF_RPC_CMD_ARGS pRpcArgs,
    PTDNF_RPC_PKGINFO_ARRAY* pRpcInfo,
    uint32_t* punCount
    )
{
    uint32_t dwError = 0;
    if(hHandle->nPrivSep)
    {
        DO_RPC(pkg_privsep_rpc_search(hHandle->hRpc,
                                      hPkgHandle,
                                      pRpcArgs,
                                      pRpcInfo,
                                      punCount),
                                      dwError);
    }
    else
    {
        DO_RPC(pkg_rpc_search(hHandle->hRpc,
                              hPkgHandle,
                              pRpcArgs,
                              pRpcInfo,
                              punCount),
                              dwError);
    }
    BAIL_ON_PMD_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;

}
uint32_t
pkg_search(
    PPMDHANDLE hHandle,
    PPKGHANDLE hPkgHandle,
    PTDNF_CMD_ARGS pCmdArgs,
    PTDNF_PKG_INFO* ppPkgInfo,
    uint32_t* punCount
    )
{
    uint32_t dwError = 0;
    PTDNF_RPC_CMD_ARGS pRpcArgs = NULL;

    PTDNF_RPC_PKGINFO_ARRAY pRpcInfo = NULL;
    PTDNF_PKG_INFO pPkgInfo = NULL;

    dwError = pkg_get_rpc_cmd_args(pCmdArgs, &pRpcArgs);
    BAIL_ON_PMD_ERROR(dwError);

    dwError = pkg_search_w(hHandle, hPkgHandle, pRpcArgs, &pRpcInfo, punCount);
    BAIL_ON_PMD_ERROR(dwError);

    dwError = PMDRpcClientConvertPkgInfo(pRpcInfo, &pPkgInfo);
    BAIL_ON_PMD_ERROR(dwError);

    *ppPkgInfo = pPkgInfo;

cleanup:
    if(pRpcArgs)
    {
        free_pkg_rpc_cmd_args(pRpcArgs);
    }
    if(pRpcInfo)
    {
        PMDRpcClientFreePkgInfoArray(pRpcInfo);
    }
    return dwError;
error:
    if(ppPkgInfo)
    {
        *ppPkgInfo = NULL;
    }
    if(pPkgInfo)
    {
        pkg_free_package_info_list(pPkgInfo);
    }
    goto cleanup;
}

uint32_t
pkg_provides_w(
    PPMDHANDLE hHandle,
    PPKGHANDLE hPkgHandle,
    wstring_t pwszSpec,
    PTDNF_RPC_PKGINFO_ARRAY* pRpcInfo
    )
{

    uint32_t dwError = 0;

    if(hHandle->nPrivSep)
    {
        DO_RPC(pkg_privsep_rpc_provides(hHandle->hRpc,
                                        hPkgHandle,
                                        pwszSpec,
                                        pRpcInfo),
                                        dwError);
    }
    else
    {
        DO_RPC(pkg_rpc_provides(hHandle->hRpc, hPkgHandle, pwszSpec, pRpcInfo),
               dwError);
    }
    BAIL_ON_PMD_ERROR(dwError);
cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
pkg_provides(
    PPMDHANDLE hHandle,
    PPKGHANDLE hPkgHandle,
    const char *pszSpec,
    PTDNF_PKG_INFO *ppPkgInfo
    )
{
    uint32_t dwError = 0;
    wstring_t pwszSpec = NULL;
    PTDNF_RPC_PKGINFO_ARRAY pRpcPkgInfo = NULL;
    PTDNF_PKG_INFO pPkgInfo = NULL;

    dwError = PMDAllocateStringWFromA(
                  pszSpec,
                  &pwszSpec);
    BAIL_ON_PMD_ERROR(dwError);

    dwError = pkg_provides_w(hHandle, hPkgHandle, pwszSpec, &pRpcPkgInfo);
    BAIL_ON_PMD_ERROR(dwError);

    dwError = PMDRpcClientConvertPkgInfoList(pRpcPkgInfo, &pPkgInfo);
    BAIL_ON_PMD_ERROR(dwError);

    *ppPkgInfo = pPkgInfo;
cleanup:
    PMD_SAFE_FREE_MEMORY(pwszSpec);
    if(pRpcPkgInfo)
    {
        PMDRpcClientFreePkgInfoArray(pRpcPkgInfo);
    }
    return dwError;
error:
    if(ppPkgInfo)
    {
        *ppPkgInfo = NULL;
    }
    if(pPkgInfo)
    {
        pkg_free_package_info_list(pPkgInfo);
    }
    goto cleanup;
}

uint32_t
pkg_list_w(
    PPMDHANDLE hHandle,
    PPKGHANDLE hPkgHandle,
    TDNF_SCOPE nScope,
    PPMD_WSTRING_ARRAY pPkgNameSpecs,
    PTDNF_RPC_PKGINFO_ARRAY* ppRpcInfo
    )
{
    uint32_t dwError = 0;
    if(!hHandle || !hPkgHandle || !pPkgNameSpecs || !ppRpcInfo)
    {
        dwError = ERROR_PMD_INVALID_PARAMETER;
        BAIL_ON_PMD_ERROR(dwError);
    }

    if(hHandle->nPrivSep)
    {
        DO_RPC(pkg_privsep_rpc_list(hHandle->hRpc,
                            hPkgHandle,
                            nScope,
                            pPkgNameSpecs,
                            ppRpcInfo), dwError);
    }
    else
    {
        DO_RPC(pkg_rpc_list(hHandle->hRpc,
                            hPkgHandle,
                            nScope,
                            pPkgNameSpecs,
                            ppRpcInfo), dwError);
    }
    BAIL_ON_PMD_ERROR(dwError);

error:
    return dwError;
}

uint32_t
pkg_list(
    PPMDHANDLE hHandle,
    PPKGHANDLE hPkgHandle,
    TDNF_SCOPE nScope,
    char **ppszPkgNameSpecs,
    PTDNF_PKG_INFO *ppPkgInfo,
    uint32_t *pdwCount
    )
{
    uint32_t dwError = 0;
    uint32_t dwCount = 0;
    uint32_t dwIndex = 0;

    PTDNF_RPC_PKGINFO_ARRAY pRpcInfo = NULL;

    PTDNF_PKG_INFO pPkgInfo = NULL;
    PPMD_WSTRING_ARRAY pPkgNameSpecs = NULL;
    char **ppszNameSpecsTemp = NULL;

    if(!ppPkgInfo || !ppszPkgNameSpecs || !pdwCount)
    {
        dwError = ERROR_PMD_INVALID_PARAMETER;
        BAIL_ON_PMD_ERROR(dwError);
    }

    ppszNameSpecsTemp = ppszPkgNameSpecs;

    dwError = PMDAllocateMemory(sizeof(PMD_WSTRING_ARRAY),
                               (void **)&pPkgNameSpecs);
    BAIL_ON_PMD_ERROR(dwError);

    while(*ppszNameSpecsTemp)
    {
        pPkgNameSpecs->dwCount++;
        ppszNameSpecsTemp++;
    }

    if(pPkgNameSpecs->dwCount > 0)
    {
        int i = 0;
        dwError = PMDAllocateMemory(sizeof(wstring_t) * pPkgNameSpecs->dwCount,
                                    (void **)&pPkgNameSpecs->ppwszStrings);
        BAIL_ON_PMD_ERROR(dwError);

        for(i = 0; i < pPkgNameSpecs->dwCount; ++i)
        {
            dwError = PMDAllocateStringWFromA(ppszPkgNameSpecs[i],
                                              &pPkgNameSpecs->ppwszStrings[i]);
            BAIL_ON_PMD_ERROR(dwError);
        }
    }

    dwError = pkg_list_w(hHandle, hPkgHandle, nScope, pPkgNameSpecs, &pRpcInfo);
    BAIL_ON_PMD_ERROR(dwError);

    dwError = PMDRpcClientConvertPkgInfo(pRpcInfo, &pPkgInfo);
    BAIL_ON_PMD_ERROR(dwError);

    *ppPkgInfo = pPkgInfo;
    *pdwCount = pRpcInfo->dwCount;

cleanup:
    if(pPkgNameSpecs)
    {
        pmd_free_wstring_array(pPkgNameSpecs);
    }
    if(pRpcInfo)
    {
        PMDRpcClientFreePkgInfoArray(pRpcInfo);
    }
    return dwError;
error:
    if(ppPkgInfo)
    {
        *ppPkgInfo = NULL;
    }
    if(pdwCount)
    {
        *pdwCount = 0;
    }
    goto cleanup;
}

uint32_t
pkg_count(
    PPMDHANDLE hHandle,
    PPKGHANDLE hPkgHandle,
    uint32_t* pdwCount
    )
{
    uint32_t dwError = 0;
    uint32_t dwCount = 0;

    if(hHandle->nPrivSep)
    {
        DO_RPC(pkg_privsep_rpc_count(hHandle->hRpc, hPkgHandle, &dwCount),
               dwError);
    }
    else
    {
        DO_RPC(pkg_rpc_count(hHandle->hRpc, hPkgHandle, &dwCount), dwError);
    }
    BAIL_ON_PMD_ERROR(dwError);

    *pdwCount = dwCount;
cleanup:
    return dwError;
error:
    if(pdwCount)
    {
        *pdwCount = 0;
    }
    goto cleanup;
}


uint32_t
pkg_repolist_w(
    PPMDHANDLE hHandle,
    PPKGHANDLE hPkgHandle,
    TDNF_REPOLISTFILTER nRepoListFilter,
    PTDNF_RPC_REPODATA_ARRAY *ppRpcRepoDataArray
    )
{
    uint32_t dwError = 0;
    if(!hHandle || !hPkgHandle || !ppRpcRepoDataArray)
    {
        dwError = ERROR_PMD_INVALID_PARAMETER;
        BAIL_ON_PMD_ERROR(dwError);
    }

    if(hHandle->nPrivSep)
    {
        DO_RPC(pkg_privsep_rpc_repolist(hHandle->hRpc,
                            hPkgHandle,
                            nRepoListFilter,
                            ppRpcRepoDataArray), dwError);
    }
    else
    {
        DO_RPC(pkg_rpc_repolist(hHandle->hRpc,
                            hPkgHandle,
                            nRepoListFilter,
                            ppRpcRepoDataArray), dwError);
    }
    BAIL_ON_PMD_ERROR(dwError);

error:
    return dwError;
}

uint32_t
pkg_repolist(
    PPMDHANDLE hHandle,
    PPKGHANDLE hPkgHandle,
    TDNF_REPOLISTFILTER nRepoListFilter,
    PTDNF_REPO_DATA *ppRepoData
    )
{
    uint32_t dwError = 0;
    uint32_t dwCount = 0;
    int i = 0;
    PTDNF_RPC_REPODATA_ARRAY pRpcRepoDataArray = NULL;
    PTDNF_RPC_REPODATA pRpcRepoData = NULL;

    PTDNF_REPO_DATA pRepoData = NULL;
    PTDNF_REPO_DATA pRepoTemp = NULL;

    if(!hHandle || !hPkgHandle || !ppRepoData)
    {
        dwError = ERROR_PMD_INVALID_PARAMETER;
        BAIL_ON_PMD_ERROR(dwError);
    }

    dwError = pkg_repolist_w(
                  hHandle,
                  hPkgHandle,
                  nRepoListFilter,
                  &pRpcRepoDataArray);
    BAIL_ON_PMD_ERROR(dwError);

    if(!pRpcRepoDataArray || !pRpcRepoDataArray->dwCount)
    {
        dwError = ERROR_PMD_NO_DATA;
        BAIL_ON_PMD_ERROR(dwError);
    }

    dwCount = pRpcRepoDataArray->dwCount;
    pRpcRepoData = pRpcRepoDataArray->pRepoData;
    for(i = dwCount-1; i >= 0; --i)
    {
        dwError = PMDAllocateMemory(sizeof(TDNF_REPO_DATA),
                                    (void**)&pRepoTemp);
        BAIL_ON_PMD_ERROR(dwError);

        pRepoTemp->nEnabled = pRpcRepoData[i].nEnabled;
        dwError = PMDAllocateStringAFromW(pRpcRepoData[i].pwszId,
                                          &pRepoTemp->pszId);
        BAIL_ON_PMD_ERROR(dwError);

        dwError = PMDAllocateStringAFromW(pRpcRepoData[i].pwszName,
                                          &pRepoTemp->pszName);
        BAIL_ON_PMD_ERROR(dwError);

        pRepoTemp->pNext = pRepoData;
        pRepoData = pRepoTemp;
        pRepoTemp = NULL;
    }
    *ppRepoData = pRepoData;

cleanup:
    if(pRpcRepoDataArray)
    {
        PMDRpcClientFreeRepoDataArray(pRpcRepoDataArray);
    }
    return dwError;
error:
    if(ppRepoData)
    {
        *ppRepoData = NULL;
    }
    pkg_free_repos(pRepoTemp);
    pkg_free_repos(pRepoData);
    goto cleanup;
}

uint32_t
pkg_updateinfo_w(
    PPMDHANDLE hHandle,
    PPKGHANDLE hPkgHandle,
    PTDNF_RPC_UPDATEINFO_SUMMARY_ARRAY *ppRpcSummary
    )
{
    uint32_t dwError = 0;
    if(!hHandle || !hPkgHandle || !ppRpcSummary)
    {
        dwError = ERROR_PMD_INVALID_PARAMETER;
        BAIL_ON_PMD_ERROR(dwError);
    }

    if(hHandle->nPrivSep)
    {
        DO_RPC(pkg_privsep_rpc_updateinfo_summary(hHandle->hRpc,
                                      hPkgHandle,
                                      ppRpcSummary), dwError);
    }
    else
    {
        DO_RPC(pkg_rpc_updateinfo_summary(hHandle->hRpc,
                                      hPkgHandle,
                                      ppRpcSummary), dwError);
    }
    BAIL_ON_PMD_ERROR(dwError);
error:
    return dwError;
}

uint32_t
pkg_updateinfo(
    PPMDHANDLE hHandle,
    PPKGHANDLE hPkgHandle,
    char **ppszPackageNameSpecs,
    PTDNF_UPDATEINFO *ppUpdateInfo
    )
{
    uint32_t dwError = 0;
    uint32_t dwIndex = 0;

    PTDNF_UPDATEINFO pUpdateInfo = NULL;
    PTDNF_RPC_UPDATEINFO_SUMMARY_ARRAY pRpcSummary = NULL;

    if(!hHandle || !ppUpdateInfo)
    {
        dwError = ERROR_PMD_INVALID_PARAMETER;
        BAIL_ON_PMD_ERROR(dwError);
    }

    dwError = pkg_updateinfo_w(hHandle, hPkgHandle, &pRpcSummary);
    BAIL_ON_PMD_ERROR(dwError);

    if(!pRpcSummary || !pRpcSummary->dwCount)
    {
        dwError = ERROR_PMD_NO_DATA;
        BAIL_ON_PMD_ERROR(dwError);
    }

    dwError = PMDAllocateMemory(
                  sizeof(TDNF_UPDATEINFO) * pRpcSummary->dwCount,
                  (void**)&pUpdateInfo);
    BAIL_ON_PMD_ERROR(dwError);

    *ppUpdateInfo = pUpdateInfo;

cleanup:
    return dwError;

error:
    if(ppUpdateInfo)
    {
        *ppUpdateInfo = NULL;
    }
    goto cleanup;
}

uint32_t
pkg_updateinfo_summary_w(
    PPMDHANDLE hHandle,
    PPKGHANDLE hPkgHandle,
    PTDNF_RPC_UPDATEINFO_SUMMARY_ARRAY *ppRpcSummary
    )
{
    uint32_t dwError = 0;
    if(!hHandle || !hPkgHandle || !ppRpcSummary)
    {
        dwError = ERROR_PMD_INVALID_PARAMETER;
        BAIL_ON_PMD_ERROR(dwError);
    }

    if(hHandle->nPrivSep)
    {
        DO_RPC(pkg_privsep_rpc_updateinfo_summary(hHandle->hRpc,
                                      hPkgHandle,
                                      ppRpcSummary), dwError);
    }
    else
    {
        DO_RPC(pkg_rpc_updateinfo_summary(hHandle->hRpc,
                                      hPkgHandle,
                                      ppRpcSummary), dwError);
    }
    BAIL_ON_PMD_ERROR(dwError);

error:
    return dwError;
}

uint32_t
pkg_updateinfo_summary(
    PPMDHANDLE hHandle,
    PPKGHANDLE hPkgHandle,
    char **ppszPackageNameSpecs,
    PTDNF_UPDATEINFO_SUMMARY *ppSummary
    )
{
    uint32_t dwError = 0;
    uint32_t dwIndex = 0;

    PTDNF_UPDATEINFO_SUMMARY pSummary = NULL;
    PTDNF_RPC_UPDATEINFO_SUMMARY_ARRAY pRpcSummary = NULL;

    if(!hHandle || !ppSummary)
    {
        dwError = ERROR_PMD_INVALID_PARAMETER;
        BAIL_ON_PMD_ERROR(dwError);
    }

    dwError = pkg_updateinfo_summary_w(
                  hHandle,
                  hPkgHandle,
                  &pRpcSummary);
    BAIL_ON_PMD_ERROR(dwError);

    if(!pRpcSummary || !pRpcSummary->dwCount)
    {
        dwError = ERROR_PMD_NO_DATA;
        BAIL_ON_PMD_ERROR(dwError);
    }

    dwError = PMDAllocateMemory(
                  sizeof(TDNF_UPDATEINFO_SUMMARY) * pRpcSummary->dwCount,
                  (void**)&pSummary);
    BAIL_ON_PMD_ERROR(dwError);

    for(dwIndex = 0; dwIndex < pRpcSummary->dwCount; ++dwIndex)
    {
        pSummary[dwIndex].nType =
            pRpcSummary->pRpcUpdateInfoSummaries[dwIndex].nType;
        pSummary[dwIndex].nCount =
            pRpcSummary->pRpcUpdateInfoSummaries[dwIndex].nCount;
    }

    *ppSummary = pSummary;

cleanup:
    return dwError;

error:
    if(ppSummary)
    {
        *ppSummary = NULL;
    }
    pkg_free_updateinfo_summary(pSummary);
    goto cleanup;
}

uint32_t
pkg_resolve_w(
    PPMDHANDLE hHandle,
    PPKGHANDLE hPkgHandle,
    TDNF_ALTERTYPE nAlterType,
    PTDNF_RPC_SOLVED_PKG_INFO *ppRpcSolvedInfo
    )
{
    uint32_t dwError = 0;
    if(!hHandle || !hPkgHandle || !ppRpcSolvedInfo)
    {
        dwError = ERROR_PMD_INVALID_PARAMETER;
        BAIL_ON_PMD_ERROR(dwError);
    }

    if(hHandle->nPrivSep)
    {
        DO_RPC(pkg_privsep_rpc_resolve(hHandle->hRpc,
                                       hPkgHandle,
                                       nAlterType,
                                       ppRpcSolvedInfo), dwError);
    }
    else
    {
        DO_RPC(pkg_rpc_resolve(hHandle->hRpc,
                               hPkgHandle,
                               nAlterType,
                               ppRpcSolvedInfo), dwError);
    }
    BAIL_ON_PMD_ERROR(dwError);

error:
    return dwError;
}

uint32_t
pkg_resolve(
    PPMDHANDLE hHandle,
    PPKGHANDLE hPkgHandle,
    TDNF_ALTERTYPE nAlterType,
    PTDNF_SOLVED_PKG_INFO *ppSolvedInfo
    )
{
    uint32_t dwError = 0;
    PTDNF_RPC_SOLVED_PKG_INFO pRpcSolvedInfo = NULL;
    PTDNF_SOLVED_PKG_INFO pSolvedInfo = NULL;

    if(!hHandle)
    {
        dwError = ERROR_PMD_INVALID_PARAMETER;
        BAIL_ON_PMD_ERROR(dwError);
    }

    dwError = pkg_resolve_w(
                  hHandle,
                  hPkgHandle,
                  nAlterType,
                  &pRpcSolvedInfo);
    BAIL_ON_PMD_ERROR(dwError);

    dwError = PMDRpcClientConvertSolvedPkgInfo(pRpcSolvedInfo, &pSolvedInfo);
    BAIL_ON_PMD_ERROR(dwError);

    *ppSolvedInfo = pSolvedInfo;

cleanup:
    return dwError;

error:
    if(ppSolvedInfo)
    {
        *ppSolvedInfo = NULL;
    }
    pkg_free_solvedinfo(pSolvedInfo);
    goto cleanup;
}

uint32_t
pkg_alter_w(
    PPMDHANDLE hHandle,
    PPKGHANDLE hPkgHandle,
    TDNF_ALTERTYPE nAlterType
    )
{
    uint32_t dwError = 0;
    if(!hHandle || !hPkgHandle)
    {
        dwError = ERROR_PMD_INVALID_PARAMETER;
        BAIL_ON_PMD_ERROR(dwError);
    }

    if(hHandle->nPrivSep)
    {
        DO_RPC(pkg_privsep_rpc_alter(hHandle->hRpc, hPkgHandle, nAlterType),
               dwError);
    }
    else
    {
        DO_RPC(pkg_rpc_alter(hHandle->hRpc, hPkgHandle, nAlterType), dwError);
    }
    BAIL_ON_PMD_ERROR(dwError);

error:
    return dwError;
}

uint32_t
pkg_alter(
    PPMDHANDLE hHandle,
    PPKGHANDLE hPkgHandle,
    TDNF_ALTERTYPE nAlterType,
    PTDNF_SOLVED_PKG_INFO pSolvedPkgInfo
    )
{
    uint32_t dwError = 0;

    if(!hHandle)
    {
        dwError = ERROR_PMD_INVALID_PARAMETER;
        BAIL_ON_PMD_ERROR(dwError);
    }

    dwError = pkg_alter_w(
                  hHandle,
                  hPkgHandle,
                  nAlterType);
    BAIL_ON_PMD_ERROR(dwError);

cleanup:
    return dwError;

error:
    goto cleanup;
}

uint32_t
pkg_version_w(
    PPMDHANDLE hHandle,
    wstring_t *ppwszVersion
    )
{
    uint32_t dwError = 0;
    if(!hHandle || !ppwszVersion)
    {
        dwError = ERROR_PMD_INVALID_PARAMETER;
        BAIL_ON_PMD_ERROR(dwError);
    }

    if(hHandle->nPrivSep)
    {
        DO_RPC(pkg_privsep_rpc_version(hHandle->hRpc, ppwszVersion), dwError);
    }
    else
    {
        DO_RPC(pkg_rpc_version(hHandle->hRpc, ppwszVersion), dwError);
    }
    BAIL_ON_PMD_ERROR(dwError);

error:
    return dwError;
}

uint32_t
pkg_version(
    PPMDHANDLE hHandle,
    char **ppszVersion
    )
{
    uint32_t dwError = 0;
    wstring_t pwszVersion = NULL;
    char *pszVersion = NULL;

    if(!hHandle)
    {
        dwError = ERROR_PMD_INVALID_PARAMETER;
        BAIL_ON_PMD_ERROR(dwError);
    }

    dwError = pkg_version_w(hHandle, &pwszVersion);
    BAIL_ON_PMD_ERROR(dwError);

    dwError = PMDAllocateStringAFromW(
                  pwszVersion,
                  &pszVersion);
    BAIL_ON_PMD_ERROR(dwError);

    *ppszVersion = pszVersion;

cleanup:
    PMDRpcClientFreeStringW(pwszVersion);
    return dwError;

error:
    if(ppszVersion)
    {
        *ppszVersion = NULL;
    }
    PMD_SAFE_FREE_MEMORY(pszVersion);
    goto cleanup;
}

uint32_t
pmd_server_type(
    PPMDHANDLE hHandle,
    uint32_t* pdwServerType
    )
{
    uint32_t dwError = 0;
    uint32_t dwServerType = 0;

    if(!hHandle || !pdwServerType)
    {
        dwError = ERROR_PMD_INVALID_PARAMETER;
        BAIL_ON_PMD_ERROR(dwError);
    }

    DO_RPC(pmd_rpc_server_type(hHandle->hRpc, &dwServerType), dwError);
    BAIL_ON_PMD_ERROR(dwError);

    *pdwServerType = dwServerType;
cleanup:
    return dwError;

error:
    if(pdwServerType)
    {
        *pdwServerType = 0;
    }
    goto cleanup;
}

void
pkg_free_repos(
    PTDNF_REPO_DATA pRepos
    )
{
    PTDNF_REPO_DATA pRepo = NULL;
    while(pRepos)
    {
        pRepo = pRepos;
        PMD_SAFE_FREE_MEMORY(pRepo->pszId);
        PMD_SAFE_FREE_MEMORY(pRepo->pszName);
        PMD_SAFE_FREE_MEMORY(pRepo->pszBaseUrl);
        PMD_SAFE_FREE_MEMORY(pRepo->pszMetaLink);
        PMDFreeStringArray(pRepo->ppszUrlGPGKeys);

        pRepos = pRepo->pNext;
        PMD_SAFE_FREE_MEMORY(pRepo);
    }
}

void
pkg_free_package_info(
    PTDNF_PKG_INFO pPkgInfo
    )
{
    if(!pPkgInfo)
    {
        return;
    }
    PMD_SAFE_FREE_MEMORY(pPkgInfo->pszName);
    PMD_SAFE_FREE_MEMORY(pPkgInfo->pszRepoName);
    PMD_SAFE_FREE_MEMORY(pPkgInfo->pszVersion);
    PMD_SAFE_FREE_MEMORY(pPkgInfo->pszArch);
    PMD_SAFE_FREE_MEMORY(pPkgInfo->pszSummary);
    PMD_SAFE_FREE_MEMORY(pPkgInfo->pszURL);
    PMD_SAFE_FREE_MEMORY(pPkgInfo->pszLicense);
    PMD_SAFE_FREE_MEMORY(pPkgInfo->pszDescription);
    PMD_SAFE_FREE_MEMORY(pPkgInfo->pszFormattedSize);
    PMD_SAFE_FREE_MEMORY(pPkgInfo->pszRelease);
}

void
pkg_free_package_info_list(
    PTDNF_PKG_INFO pPkgInfo
    )
{
    PTDNF_PKG_INFO pTemp = NULL;
    if(!pPkgInfo)
    {
        return;
    }
    while(pPkgInfo)
    {
        pTemp = pPkgInfo->pNext;
        pkg_free_package_info(pPkgInfo);
        PMD_SAFE_FREE_MEMORY(pPkgInfo);
        pPkgInfo = pTemp;
    }
}

void
pkg_free_package_info_array(
    PTDNF_PKG_INFO pPkgInfo,
    uint32_t dwLength
    )
{
    uint32_t i = 0;
    if(pPkgInfo && dwLength > 0)
    {
        for(i = 0; i < dwLength; ++i)
        {
            pkg_free_package_info(&pPkgInfo[i]);
        }
    }
    PMD_SAFE_FREE_MEMORY(pPkgInfo);
}

uint32_t
pkg_get_error_string_w(
    PPMDHANDLE hHandle,
    uint32_t dwErrorCode,
    wstring_t *ppwszError
    )
{
    uint32_t dwError = 0;
    if(!hHandle || !ppwszError)
    {
        dwError = ERROR_PMD_INVALID_PARAMETER;
        BAIL_ON_PMD_ERROR(dwError);
    }

    if(hHandle->nPrivSep)
    {
        DO_RPC(pkg_privsep_rpc_get_error_string(
                   hHandle->hRpc, dwErrorCode, ppwszError),
                   dwError);
    }
    else
    {
        DO_RPC(pkg_rpc_get_error_string(
                   hHandle->hRpc, dwErrorCode, ppwszError),
                   dwError);
    }
    BAIL_ON_PMD_ERROR(dwError);

error:
    return dwError;
}

uint32_t
pkg_get_error_string(
    PPMDHANDLE hHandle,
    uint32_t dwErrorCode,
    char** ppszError
    )
{
    uint32_t dwError = 0;
    wstring_t pwszError = NULL;
    char *pszError = NULL;

    if(!hHandle)
    {
        dwError = ERROR_PMD_INVALID_PARAMETER;
        BAIL_ON_PMD_ERROR(dwError);
    }

    dwError = pkg_get_error_string_w(
                  hHandle,
                  dwErrorCode,
                  &pwszError);
    BAIL_ON_PMD_ERROR(dwError);

    dwError = PMDAllocateStringAFromW(
                  pwszError,
                  &pszError);
    BAIL_ON_PMD_ERROR(dwError);

    *ppszError = pszError;

cleanup:
    PMDRpcClientFreeStringW(pwszError);
    return dwError;

error:
    if(ppszError)
    {
        *ppszError = NULL;
    }
    PMD_SAFE_FREE_MEMORY(pszError);
    goto cleanup;
}

void
pmd_free_wstring_array(
    PPMD_WSTRING_ARRAY pArray
    )
{
    if(!pArray)
    {
        return;
    }
    while(pArray->dwCount)
    {
        PMDFreeMemory(pArray->ppwszStrings[--pArray->dwCount]);
    }
    PMD_SAFE_FREE_MEMORY(pArray->ppwszStrings);
    PMDFreeMemory(pArray);
}

void
pkg_free_updateinfo_summary(
    PTDNF_UPDATEINFO_SUMMARY pSummary
    )
{
    if(pSummary)
    {
        PMD_SAFE_FREE_MEMORY(pSummary);
    }
}

void
pkg_free_solvedinfo(
    PTDNF_SOLVED_PKG_INFO pSolvedInfo
    )
{
    if(pSolvedInfo)
    {
        pkg_free_package_info_list(pSolvedInfo->pPkgsExisting);
        pkg_free_package_info_list(pSolvedInfo->pPkgsToInstall);
        pkg_free_package_info_list(pSolvedInfo->pPkgsToUpgrade);
        pkg_free_package_info_list(pSolvedInfo->pPkgsToDowngrade);
        pkg_free_package_info_list(pSolvedInfo->pPkgsNotAvailable);
        pkg_free_package_info_list(pSolvedInfo->pPkgsToRemove);
        pkg_free_package_info_list(pSolvedInfo->pPkgsToReinstall);
        PMDFreeStringArray(pSolvedInfo->ppszPkgsNotResolved);
        PMD_SAFE_FREE_MEMORY(pSolvedInfo);
    }
}
