# SBuilder Repository

SBuilder is a tool for browsing and editing SQLite databases.  Its goal is to
provide details on every aspect of a database file.  The kind of details that
an application developer who is using SQLite would be interested in.

SBuilder is an Open Source project under the [Apache 2.0
License](http://www.apache.org/licenses/LICENSE-2.0.html)

## Code Organization

The code is broken up as follows:
* src/ contains UI code for SBuilder
* src/core contains an OO wrapper for SQLite that is used by the UI code
* src/sqlite contains the SQLite library
* src/images tango desktop icons used in the application
* test/ contains code to create databases that can be opened in SBuilder to
  test different functionality

## Contributing Code

If you wish to contribute bug fixes or features:

1. Fork the **sbuilder** repository
2. Make the changes/additions to your fork
3. Send a pull request from your fork back to the **sbuilder** repository
4. A committer (listed below) will review your patch and optionally pull it in

## Governence

The code for sbuilder is managed by the committers (listed below).  When a pull
request comes in for a specific bug or feature one of the committers will review
the code and optionally pull it into the sbuilder repository.

**Repository Committers** 

* [Nick Landry](https://github.com/nlandry)

## Bug Reporting and Feature Requests

If you find a bug, or have an enhancement request, simply file an [Issue].

## Disclaimer

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
