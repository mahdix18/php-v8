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

/**
 * A JavaScript value representing a 32-bit unsigned integer.
 */
class Uint32Value extends IntegerValue
{
    /**
     * @param Isolate $isolate
     * @param int     $value Should be valid uint32 value
     */
    public function __construct(Isolate $isolate, int $value)
    {
        parent::__construct($isolate);
    }

    /**
     * @return int uint32 value
     */
    public function Value(): int
    {
    }
}
