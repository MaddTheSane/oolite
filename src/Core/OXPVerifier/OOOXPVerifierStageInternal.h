/*

OOOXPVerifierStageInternal.h

Private interface between OOOXPVerifierStage and OOOXPVerifier.


Copyright (C) 2007-2013 Jens Ayton

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#import "OOOXPVerifierStage.h"

#if OO_OXP_VERIFIER_ENABLED

@interface OOOXPVerifierStage (OOInternal)

- (void)setVerifier:(OOOXPVerifier *)verifier;
- (BOOL)isDependentOf:(OOOXPVerifierStage *)stage;
- (void)registerDependency:(OOOXPVerifierStage *)dependency;
- (void)dependencyRegistrationComplete;

@property (readonly) BOOL canRun;

- (void)performRun;
- (void)noteSkipped;

// These return sets of stages set up by -registerDependency, wheras -dependencies/dependents return sets of names.
@property (readonly, copy) NSSet *resolvedDependencies;
@property (readonly, copy) NSSet *resolvedDependents;

@end

#endif
