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


#pragma once

#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <termios.h>
#include <sys/wait.h>
#include <locale.h>
#include <iconv.h>

#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>

#include <pmderror.h>
#include <dce/rpc.h>
#include <dce/dcethread.h>
#include <dce/dce_error.h>

#include <curl/curl.h>
#include <roleplugin.h>
#include "defines.h"
#include "structs.h"
#include "prototypes.h"
#include "../server/defines.h"
#include <pmd_fwmgmt_types.h>
#include <pmd_rolemgmt_types.h>
#include <pmd_usermgmt_types.h>

#include "gssapi_creds_plugin.h"
