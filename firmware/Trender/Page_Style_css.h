/**
 *  This file is part of Trender.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2016 Jérôme Labidurie jerome@labidurie.fr
 *  Copyright 2016 Cédric Bernard cedric.bernard@galilabs.com
 *  Copyright 2016 Julien Jacques julien.jacques@galilabs.com
 */

const char PAGE_Style_css[]  = R"=====(
body { color: #000000; font-family: avenir, helvetica, arial, sans-serif;  letter-spacing: 0.15em;}
hr {    background-color: #eee;    border: 0 none;   color: #eee;    height: 1px; }
.btn, .btn:link, .btn:visited {
  border-radius: 0.3em;
  border-style: solid;
  border-width: 1px;
color: #111;
display: inline-block;
  font-family: avenir, helvetica, arial, sans-serif;
  letter-spacing: 0.15em;
  margin-bottom: 0.5em;
padding: 1em 0.75em;
  text-decoration: none;
  text-transform: uppercase;
  -webkit-transition: color 0.4s, background-color 0.4s, border 0.4s;
transition: color 0.4s, background-color 0.4s, border 0.4s;
}
.btn:hover, .btn:focus {
color: #7FDBFF;
border: 1px solid #7FDBFF;
  -webkit-transition: background-color 0.3s, color 0.3s, border 0.3s;
transition: background-color 0.3s, color 0.3s, border 0.3s;
}
  .btn:active {
color: #0074D9;
border: 1px solid #0074D9;
    -webkit-transition: background-color 0.3s, color 0.3s, border 0.3s;
transition: background-color 0.3s, color 0.3s, border 0.3s;
  }
  .btn--s
  {
    font-size: 12px;
  }
  .btn--m {
    font-size: 14px;
  }
  .btn--l {
    font-size: 20px;  border-radius: 0.25em !important;
  }
  .btn--full, .btn--full:link {
    border-radius: 0.25em;
display: block;
      margin-left: auto;
      margin-right: auto;
      text-align: center;
width: 100%;
  }
  .btn--blue:link, .btn--blue:visited {
color: #fff;
    background-color: #0074D9;
  }
  .btn--blue:hover, .btn--blue:focus {
color: #fff !important;
    background-color: #0063aa;
    border-color: #0063aa;
  }
  .btn--blue:active {
color: #fff;
    background-color: #001F3F;  border-color: #001F3F;
  }
  @media screen and (min-width: 32em) {
    .btn--full {
      max-width: 16em !important; }
  }
)=====";


