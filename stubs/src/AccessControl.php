<?php

/**
 * This file is part of the pinepain/php-v8 PHP extension.
 *
 * Copyright (c) 2015-2017 Bogdan Padalko <pinepain@gmail.com>
 *
 * Licensed under the MIT license: http://opensource.org/licenses/MIT
 *
 * For the full copyright and license information, please view the
 * LICENSE file that was distributed with this source or visit
 * http://opensource.org/licenses/MIT
 */


namespace V8;

class AccessControl
{
    const DEFAULT_ACCESS = 0; // do not allow cross-context access
    const ALL_CAN_READ = 1; // all cross-context reads are allowed
    const ALL_CAN_WRITE = 2; // all cross-context writes are allowed
    // ALL_CAN_READ | ALL_CAN_WRITE could be use allow all cross-context access
}
