/*
 * Copyright © 2016-2017 VMware, Inc.  All Rights Reserved.
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
ncmcli_get_version(
    PPMDHANDLE hPMD,
    int argc,
    char *argv[]
)
{
    uint32_t dwError = 0;
    char *pszGetVersion = NULL;

    if(!hPMD)
    {
        dwError = ERROR_PMD_INVALID_PARAMETER;
        BAIL_ON_CLI_ERROR(dwError);
    }
    dwError = netmgr_client_get_version(hPMD, &pszGetVersion);
    BAIL_ON_CLI_ERROR(dwError);
    fprintf(stdout, "network-config-manager %s\n", pszGetVersion);
cleanup:
    /* Free allocated memory */
    PMD_SAFE_FREE_MEMORY(pszGetVersion);
    return dwError;
error:
    goto cleanup;
}

uint32_t
ncmcli_is_networkd_running(
    PPMDHANDLE hPMD,
    int argc,
    char *argv[]
)
{
    uint32_t dwError = 0;
    char *pszIsNetworkdRunning = NULL;

    if(!hPMD)
    {
        dwError = ERROR_PMD_INVALID_PARAMETER;
        BAIL_ON_CLI_ERROR(dwError);
    }
    dwError = netmgr_client_is_networkd_running(hPMD, &pszIsNetworkdRunning);
    BAIL_ON_CLI_ERROR(dwError);
    fprintf(stdout, "systemd-networkd is %s\n", pszIsNetworkdRunning);
cleanup:
    /* Free allocated memory */
    PMD_SAFE_FREE_MEMORY(pszIsNetworkdRunning);
    return dwError;
error:
    goto cleanup;
}


uint32_t
ncmcli_configure(
    PPMDHANDLE hPMD,
    int argc,
    char *argv[]
)
{
    uint32_t dwError = 0;

    if(!hPMD || (argc <= 0) || !argv)
    {
        dwError = ERROR_PMD_INVALID_PARAMETER;
        BAIL_ON_CLI_ERROR(dwError);
    }
    dwError = netmgr_client_configure(hPMD, argc, (const char **)argv);
    BAIL_ON_CLI_ERROR(dwError);

cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
ncmcli_link_get_dhcp_client_iaid(
    PPMDHANDLE hPMD,
    int argc,
    char *argv[]
)
{
    uint32_t dwError = 0;
    uint32_t nIaid = 0;

    if(!hPMD || !argv[1])
    {
        dwError = ERROR_PMD_INVALID_PARAMETER;
        BAIL_ON_CLI_ERROR(dwError);
    }
    dwError = netmgr_client_get_dhcp_client_iaid(
			hPMD,
                        argv[1],
			&nIaid);
    BAIL_ON_CLI_ERROR(dwError);

    fprintf(stdout, "IAID=%u\n", nIaid);
cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
ncmcli_link_get_mtu(
    PPMDHANDLE hPMD,
    int argc,
    char *argv[]
)
{
    uint32_t dwError = 0;
    uint32_t nMtu = 0;

    if(!hPMD || !argv[1])
    {
        dwError = ERROR_PMD_INVALID_PARAMETER;
        BAIL_ON_CLI_ERROR(dwError);
    }
    dwError = netmgr_client_get_link_mtu(
			hPMD,
                        argv[1],
			&nMtu);
    BAIL_ON_CLI_ERROR(dwError);

    fprintf(stdout, "MTU=%u\n", nMtu);
cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
ncmcli_link_get_mac_addr(
    PPMDHANDLE hPMD,
    int argc,
    char *argv[]
)
{
    uint32_t dwError = 0;
    char *ppszGetMacAddr = NULL;

    if(!hPMD || !argv[1])
    {
        dwError = ERROR_PMD_INVALID_PARAMETER;
        BAIL_ON_CLI_ERROR(dwError);
    }
    dwError = netmgr_client_get_mac_addr(hPMD, argv[1], &ppszGetMacAddr);
    BAIL_ON_CLI_ERROR(dwError);
    fprintf(stdout, "MacAddress=%s\n", ppszGetMacAddr);
cleanup:
    /* Free allocated memory */
    PMD_SAFE_FREE_MEMORY(ppszGetMacAddr);
    return dwError;
error:
    goto cleanup;
}

uint32_t
ncmcli_link_get_dhcp_mode(
    PPMDHANDLE hPMD,
    int argc,
    char *argv[]
)
{
    uint32_t dwError = 0;
    uint32_t nDHCPMode = 0;

    if(!hPMD || !argv[1])
    {
        dwError = ERROR_PMD_INVALID_PARAMETER;
        BAIL_ON_CLI_ERROR(dwError);
    }
    dwError = netmgr_client_get_dhcp_mode(
			hPMD,
                        argv[1],
			&nDHCPMode);
    BAIL_ON_CLI_ERROR(dwError);

    fprintf(stdout, "DHCPMode=%s\n", net_dhcp_modes_to_name(nDHCPMode));
cleanup:
    return dwError;
error:
    goto cleanup;
}

uint32_t
ncmcli_link_get_addresses(
    PPMDHANDLE hPMD,
    int argc,
    char *argv[]
)
{
    uint32_t dwError = 0, i = 0;
    char **ppszAddresses = NULL;
    size_t nCount = 0;

    if(!hPMD || !argv[1])
    {
        dwError = ERROR_PMD_INVALID_PARAMETER;
        BAIL_ON_CLI_ERROR(dwError);
    }
    dwError = netmgr_client_get_addresses(hPMD, argv[1], &nCount, &ppszAddresses);
    BAIL_ON_CLI_ERROR(dwError);
    fprintf(stdout, "inet address:");
    for(i = 0; i < nCount; ++i)
    {
	fprintf(stdout,
		" %s",
		ppszAddresses[i]);
    }
    fprintf(stdout, "\n");

cleanup:
    /* Free allocated memory */
    PMDFreeStringArrayWithCount(ppszAddresses, nCount);
    return dwError;
error:
    goto cleanup;
}

uint32_t
ncmcli_link_get_routes(
    PPMDHANDLE hPMD,
    int argc,
    char *argv[]
)
{
    uint32_t dwError = 0, i = 0;
    char **ppszRoutes = NULL;
    size_t nCount = 0;

    if(!hPMD || !argv[1])
    {
        dwError = ERROR_PMD_INVALID_PARAMETER;
        BAIL_ON_CLI_ERROR(dwError);
    }
    dwError = netmgr_client_get_routes(hPMD, argv[1], &nCount, &ppszRoutes);
    BAIL_ON_CLI_ERROR(dwError);
    fprintf(stdout, "Routes:");
    for(i = 0; i < nCount; ++i)
    {
	fprintf(stdout,
		" %s",
		ppszRoutes[i]);
    }
    fprintf(stdout, "\n");

cleanup:
    /* Free allocated memory */
    PMDFreeStringArrayWithCount(ppszRoutes, nCount);
    return dwError;
error:
    goto cleanup;
}

uint32_t
ncmcli_link_get_ntp(
    PPMDHANDLE hPMD,
    int argc,
    char *argv[]
)
{
    uint32_t dwError = 0, i = 0;
    char **ppszServers = NULL;
    size_t nServers = 0;

    if(!hPMD || !argv[1])
    {
        dwError = ERROR_PMD_INVALID_PARAMETER;
        BAIL_ON_CLI_ERROR(dwError);
    }
    dwError = netmgr_client_get_ntp_servers(hPMD, argv[1], &nServers, &ppszServers);
    BAIL_ON_CLI_ERROR(dwError);
    fprintf(stdout, "NTPServers=");
    for(i = 0; i < nServers; ++i)
    {
	fprintf(stdout,
		"%s%s",
		ppszServers[i],
		(i+1) < nServers ? "," : "");
    }
    fprintf(stdout, "\n");

cleanup:
    /* Free allocated memory */
    PMDFreeStringArrayWithCount(ppszServers, nServers);
    return dwError;
error:
    goto cleanup;
}

uint32_t
ncmcli_get_dns_domains(
    PPMDHANDLE hPMD,
    int argc,
    char *argv[]
)
{
    uint32_t dwError = 0;
    size_t dwCount = 0, i = 0;
    char *pszDnsDomains = NULL, **ppszDnsDomains = NULL;

    if(!hPMD)
    {
        dwError = ERROR_PMD_INVALID_PARAMETER;
        BAIL_ON_CLI_ERROR(dwError);
    }
    dwError = netmgr_client_get_dns_domains(
			hPMD,
			&dwCount,
			&ppszDnsDomains);
    BAIL_ON_CLI_ERROR(dwError);

    fprintf(stdout, "Domains=");
    for (i = 0; i < dwCount; i++)
    {
	fprintf(stdout, "%s ", ppszDnsDomains[i]);
    }
    fprintf(stdout, "\n");
cleanup:
    /* Free allocated memory */
    PMDFreeStringArrayWithCount(ppszDnsDomains, dwCount);
    return dwError;
error:
    goto cleanup;
}

uint32_t
ncmcli_get_dns_server(
    PPMDHANDLE hPMD,
    int argc,
    char *argv[]
)
{
    uint32_t dwError = 0;
    size_t dwCount = 0, i = 0;
    char *s1, *s2, *pszServers = NULL, **ppszDnsServersList = NULL;

    if(!hPMD)
    {
        dwError = ERROR_PMD_INVALID_PARAMETER;
        BAIL_ON_CLI_ERROR(dwError);
    }
    dwError = netmgr_client_get_dns_servers(
                    hPMD,
                    &dwCount,
                    &ppszDnsServersList);
    BAIL_ON_CLI_ERROR(dwError);
    fprintf(stdout, "DNSServers=");
    for (i = 0; i < dwCount; i++)
    {
	fprintf(stdout, "%s ", ppszDnsServersList[i]);
    }
    fprintf(stdout, "\n");

cleanup:
    /* Free allocated memory */
    PMDFreeStringArrayWithCount(ppszDnsServersList, dwCount);
    return dwError;
error:
    goto cleanup;
}

uint32_t
ncmcli_get_system_hostname(
    PPMDHANDLE hPMD,
    int argc,
    char *argv[]
)
{
    uint32_t dwError = 0;
    char *pszHostname = NULL;

    if(!hPMD)
    {
        dwError = ERROR_PMD_INVALID_PARAMETER;
        BAIL_ON_CLI_ERROR(dwError);
    }
    dwError = netmgr_client_get_hostname(hPMD, &pszHostname);
    BAIL_ON_CLI_ERROR(dwError);
    fprintf(stdout, "Hostname: %s\n", pszHostname);

cleanup:
    /* Free allocated memory */
    PMD_CLI_SAFE_FREE_MEMORY(pszHostname);
    return dwError;
error:
    goto cleanup;
}


static uint32_t
ncmcli_parse_argv(
    int argc,
    char * const* argv
    )
{
	uint32_t dwError = 1;
        static const struct option options[] = {
                { "help",       no_argument,       NULL, 'h' },
                { "version",    no_argument,       NULL, 'v' },
                { "servername", required_argument, NULL, 's' },
                { "user",       required_argument, NULL, 'u' },
                {}
        };
        char c;

	if ((argc < 0) || !argv)
	{
		dwError = ERROR_PMD_INVALID_PARAMETER;
		BAIL_ON_CLI_ERROR(dwError);
	}
        optind = 0;
        while ((c = getopt_long(argc, argv, "hvs:u:", options, NULL)) >= 0)
        {

                switch (c)
                {

                case 'h':
			dwError = net_show_help();
			BAIL_ON_CLI_ERROR(dwError);
			goto cleanup;

                case 'v':
                        strcpy(argv[2], "version");
			goto cleanup;

                case 's':
                case 'u':
			goto cleanup;

                default:
                        dwError = ERROR_PMD_INVALID_PARAMETER;
			BAIL_ON_CLI_ERROR(dwError);
                }
        }
cleanup:
	return dwError;
error:
	goto cleanup;
}

uint32_t
netmgr_main(
    int argc,
    char* const* argv,
    PPMD_CMD_ARGS pMainArgs)
{
    uint32_t dwError = 0;
    PPMDHANDLE hPMD = NULL;
    NetCliManager *pNetCliMgr = NULL;

    dwError = ncmcli_parse_argv(argc, argv);
    if (dwError == ERROR_PMD_INVALID_PARAMETER)
    {
	BAIL_ON_CLI_ERROR(dwError);

    }
    else if (dwError == 0)
    {
	goto cleanup;
    }
    if (optind > 1)
    {
        argc = optind - 1;
    }
    dwError = net_cli_manager_new(&pNetCliMgr);
    BAIL_ON_CLI_ERROR(dwError);

    dwError = rpc_open(
                      "net",
                      pMainArgs->pszServer,
                      pMainArgs->pszUser,
                      pMainArgs->pszDomain,
                      pMainArgs->pszPass,
                      pMainArgs->pszSpn,
                      &hPMD);
    BAIL_ON_CLI_ERROR(dwError);

    dwError = net_cli_run_command(pNetCliMgr, hPMD, argc, (char **)argv);
    BAIL_ON_CLI_ERROR(dwError);
cleanup:
    if (pNetCliMgr)
    {
	net_cli_unrefp(&pNetCliMgr);
    }
    if(hPMD)
    {
        rpc_free_handle(hPMD);
    }
    return dwError;

error:
    if(net_print_error(dwError) == 0)
    {
	/* already handled */
        dwError = ERROR_PMD_FAIL;
    }
    goto cleanup;
}

